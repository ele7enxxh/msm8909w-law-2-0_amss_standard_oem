/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    M M G S D I   S E S S I O N   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the ICC protocol processing support for MMGSDI.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_session.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/02/16   gm      Support for CDMALess Device
07/11/16   bcho    Remove unnecessary critical section leave code
07/08/16   av      Send SELECT ISDR to determine if card is eUICC or not
05/26/16   sp      F3 logging improvements
05/24/16   ar      Review of macros used by MMGSDI
05/22/16   vdc     Remove F3 messages for memory allocation failure
05/16/16   tkl     F3 log prints cleanup
05/16/16   ar      F3 reduction
05/11/16   sp      Reduce F3 logging
05/04/16   lm      Fail on going refresh if deactivation is in progress
05/02/16   nr      Don't reject activation req during ecc call for AP centric
04/29/16   gm      Error code for card not supporting select by partial AID
04/27/16   vdc     Block select AID only for ARA-M AID
04/05/16   vdc     Send act resp and session changed evt at same time
01/27/16   av      Get client_id and session_id indices when failing session open
01/12/16   av      Clean up the index when rejecting session open request
01/08/16   vdc     Pass req hdr slot id while processing select AID response
12/02/15   nr      Handling of 2 de-activation req when queued for same session
11/23/15   av      Block SELECT on AID only if first SELECT failed with 6A82
11/12/15   vdc     Fix KW issue for AID check
11/12/15   ar      Handle exclusive non prov session close on a prov app
10/16/15   ar      Don't allow reSELECT on AID that failed atleast once earlier
10/12/15   bcho    Reject internal deactivate req if APP RESET AUTO is running
09/25/15   vr      Added support for ERA-GLONASS
08/10/15   vdc     Fix potential buffer overflow in cdma AKA cnf
07/18/15   bcho    Send SELECT AID to card for non prov session on ICC card
07/17/15   ar      Update num_connection when a session mapped to channel_index
07/14/15   ar      Reset app_termination_skipped for app reactvtd via switching
07/14/15   ar      Handle sending double confirmation to client
06/18/15   bcho    Close channel if session is activated on another slot
06/04/15   lxu     Fix logical channel exhausted before CSIM activation
05/29/15   hh      Convert MANAGE CHANNEL APDU to MMGSDI requests
05/04/15   ar      Ability to process app deact while activation is in progress
03/30/15   ar      Support for cross-mapping in multisim_auto_provisioning NV
03/25/15   ar      Allow back to DEACT of different apps on same slot
12/19/14   bcho    NULL check added before dereferencing app info ptr
12/09/14   stv     Fix for subscription switch from SIM to USIM
11/24/14   vv      Send simlock run algo request if pre-perso caching is done
11/12/14   vdc     Send card error evt if NO_ATR is received after slot power up
10/23/14   yt      Updates to session and app management
09/09/14   kk      Enhancements in NV handling
08/29/14   yt      Send STATUS only before USIM/CSIM deactivation
08/08/14   ar      Queue SESSION_DEACTIVATE_REQ to high priority queue
08/06/14   ar      Replace MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE with new macro
07/30/14   vv      Added support to indicate app is initialized in recovery
07/15/14   yt      Support for NV refresh
07/07/14   vv      Remove Perso code
07/07/14   bcho    Handling of session open/close for prov/non-prov type
06/23/14   yt      Send STATUS before app deactivation
06/14/14   kk      Fixed channel data handling after app termination is skipped
06/05/14   bcho    Queue MMGSDI_SESSION_SELECT_AID_REQ to slot specific task
06/03/14   tl      Remove additional parameter variables
06/03/14   yt      Fix for switch provisioning after app termination is skipped
06/02/14   tl      Allow only USIM on default channel when manually prov.
05/14/14   hh      Reject UICC app activation if it is already being activated
05/12/14   bcho    NULL check added before dereferencing extra_param_ptr
05/08/14   hh      Correctly handle act_or_switch that results app deactivation
04/15/14   ar      Skip sending CNF if there is CARD_ERROR while processing
03/27/14   yt      Use correct slot
03/26/14   tl      SIM Lock secondary revision
03/25/14   yt      Handle refresh of deactivated app
03/21/14   tl      Introduce new SIM Lock feature
03/07/14   yt      Allow SELECT AID request with zero-length AID
02/25/14   yt      Support for selection mode in SELECT AID request
02/25/14   bcho    Added support to store OPLMN List in EFS and cache it
02/24/14   ar      Replace session checks in switch with utility functions
02/13/14   at      Fix for KW Critical errors
02/10/14   ar      Replace session checks with utility functions
01/11/14   df      Fix off-target compilation error
01/10/14   df      Use v2.0 diag macros
01/09/14   ar      Replace old Macros with MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE
01/06/14   ar      Get Index in client request table when Terminate App is True
01/02/13   ar      Featurization for 3GPP builds
12/18/13   yt      Skip app termination as part of session deactivation
12/16/13   yt      Support for ISIM AUTH in HTTP digest security context
12/11/13   df      Remove unused memory paging feature
11/18/13   vv      Added support for parallel REFRESH
11/05/13   tkl     Add IMSI check for TDS test SIM
10/29/13   vdc     Add F3 message for total & available logical channels
10/04/13   vdc     F3 message reduction
09/27/13   tl      Fix KW errors
09/11/13   kk      Fixed prov session deactivation handling
09/05/13   yt      Check first byte of ISIM auth rsp only in case of success
08/30/13   yt      Remove usage of rand() to generate client and session IDs
08/30/13   tl      Convert channel info table from static to dynamic allocation
07/15/13   yt      Fix compilation error in previous check-in
07/15/13   ssr     Fixed 62XX status byte for APP selection through
                   QMI_UIM_LOGICAL_CHANNE
06/28/13   vdc     When closing non-prov sessions, compare App types & AIDs
06/24/13   vdc     Added support for triple SIM
06/14/13   vdc     Fix non-prov session open of apps not having entry in EF-DIR
05/20/13   vv      Added support to check if a logical channel can be re-used
05/16/13   vdc     Replace memcpy with safer version memscpy
05/14/13   vdc     Memory optimizations for slot, app, pin, upin tables
04/09/13   kb      session close handling updated to free non prov session info
04/08/13   bcho    Provide response cb while queueing intenal deactivate req
04/04/13   bcho    Deactivate provisioning session if cmd queued internally
04/01/13   vv      Reject exclusive channel open request, if app is selected
                   and not shareable
02/25/13   vv      Added support to save AID received in SELECT response
01/21/13   bcho    Free the memory allocated to EONS info while releasing app
                   info for non-prov. and card sessions.
12/15/12   bcho    Set select rsp type to RSP NO DATA in channel info cleanup,
                   copy sel res type in app info while populating session info
10/11/12   spo     Removed unnecessary F3 messages
10/03/12   abg     Removed Queuing related F3 messages
09/28/12   abg     Updated ERR to UIM_MSG_ERR_0
09/19/12   vv      Reserve logical channel for CSIM app
09/14/12   vv      Check the channel state during a non-prov session open request
08/16/12   tl      Add support to close channel without app termination
07/30/12   yt      Check if app is shareable across channels
07/30/12   vv      Fixed overriding apps with zero aid length in app info table
07/25/12   bcho    Change channel status according session open/close operation
07/19/12   tl      Increase number of applications in mmgsdi context to 20
07/13/12   vv      Remove apps not in EF-DIR from app table upon deactivation
06/14/12   bcho    Memory leak removed from mmgsdi_session_release_app_info()
05/23/12   vv      Added support for the file update notification registration
04/30/12   kk      Fixed crash when session close fails at app deactivation
05/02/12   yt      Ignore warnings returned by card for SELECT AID command
04/09/12   yt      Allow opening of non-prov session in ICC mode
04/04/12   shr     Do not allow non-prov. Session Open with MF requests
                   when card is not inserted in corresponding slot
03/29/12   shr     Added support for session open with MF,
                   support for non-prov. app selection using zero length and
                   partial AID, do not allow non-prov. Session open
                   requests on ICC cards
03/23/12   vs      Return status words to client only if rsp is from card
03/20/12   yt      Add request type for session_open_with_select_rsp
03/14/12   nmb     Ensure QMI response for logical channel failures
03/12/12   vv      Fixed klocwork errors
03/07/12   bcho    NULL check removed for mmgsdi_channel_info_table_ptr
02/23/12   av      Cleaned up remaining always ON features
02/23/12   bcho    Critical Section Enter/Leave function calls replaced by
                   respective macro to add logging
01/05/12   shr     Removed F3 logging from within critical sections
12/21/11   shr     Legacy GSDI removal updates
12/21/11   bcho    Changes for new API to open multiple sessions
                   in a single request
12/21/11   bcho    Added support for Get Operator Name API
10/03/11   vv      Insert AIDs starting from index 0 of the app info table
09/16/11   av      Replace feature #ifdefs with an NVITEM
09/06/11   shr     Do not attempt a Card Reset as part of EFRESH RESET/Card
                   power-up handling, if the REFRESH has been stopped
08/11/11   vs      Close channel even if app termination fails
08/11/11   nmb     Expanded logical channels
08/02/11   av      Fixed incorrect slot_id being sent out in the session_open
                   and session_close cnfs
08/01/11   vs      Added support for session open with select response
07/11/11   shr     Fixed incorrect App Type being sent out with Non-prov
                   Session Open confirmation
06/21/11   kk      ThreadSafe updates from review
06/20/11   nmb     Remove AID info from channel table for shared channels
05/17/11   kk      Updates for Thread safety
05/17/11   shr     Updates for Thread safety
05/13/11   nmb     Added support for ISIM GBA Authenticatio
05/04/11   ms      Send the PERSO_EVT to the late clients in perso state
                   as WAIT_FOR_DEPERSO_S and INIT_ERROR_S
04/14/11   shr     If Prov. App activation fails with SW1/SW2 as 0x69/0x99
                   or with Technical Problem status, reset the card
                   and change protocol to ICC.
                   For any other app acitvation faliure cause, close any
                   channel opened
04/01/11   vs      Return status for failed manage channel and select AID
03/22/11   ea      Remove check for AID in EF-DIR
03/11/11   ms      Clear the app capabilities in session deactivation
02/21/11   ms      Implementation of Session based caching on App capabilities
02/03/11   ms      Updated F3 messages for Client ID
01/17/10   yt      Indicate that status word is present in ISIM Auth response
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
11/27/10   nmb     Updated ISIM Auth to correct possible out of bounds read
11/05/10   yt      Klocwork fixes
10/14/10   yt      Updated handling of client req table entries in case of error
10/05/10   nmb     Removed extra bytes from task_cmd buffer
09/16/10   shr     Free memory allocated for RERFRESH file registration
                   only if a session is being closed
09/15/10   nmb     Allocate mmgsdi_client_id_reg_table dynamically
09/02/10   shr     Clean up Client ID dereg and Session close handling
08/30/10   shr     Perso clean up for Sessions
08/06/10   shr     Fixed opening of a non-exclusive non-prov session for
                   an app already activated on the card
07/14/10   js      Fixed compiler warnings
07/02/10   ssr     Fixed refresh for ICC card which have both GSM and RUIM
06/24/10   yt      Klocwork fixes
06/16/10   shr     Need to reject an activate/switch request on ICC cards
                   if Session type does not match any of the DFs available
06/10/10   shr     Allow close of Card Sessions
06/01/10   shr     Send STATUS command indicating app termination to card
                   before initiating card RESET
05/24/10   shr     Need to send confirmation to client when Open channel fails
05/14/10   nmb     Fixed compilation error
05/12/10   jk      Null check for Channel App Data before cleanup
04/26/10   shr     Need to handle early opening of Card Sessions correctly
04/06/10   shr     Clean up REFRESH RESET and REFRESH RESET_AUTO
04/07/10   yt      Fixed compilation warnings
03/30/10   shr     Fixed non-prov session close handling
03/30/10   shr     Fixed Card Session open handling
03/29/10   shr     Switch/deactivate provisioning clean up and redesign
03/24/10   kk      Fixed non prov select aid error handling
03/23/10   kk      Fixed non prov session open error handling
03/19/10   vs      Fix for non-prov sessions opened to prov apps
03/12/10   adp     Fixed Klockwork errors
03/09/10   shr     Clean up REFRESH FCN, INIT, INIT_FCN and APP_RESET
02/26/10   kk      Fixed session open handling
02/17/10   shr     Fixed Session activate/deactivate handling
01/19/10   ssr     Fixed session close issue where channel_info_ptr is null
01/06/10   shr     Fixed Session Close handling
12/02/09   js      Fixed session open issue for non-prov type sessions with
                   exclusive channel
12/02/09   js      Fixes for RUIM power up
09/26/09   kp      ZI memory reduction changes
04/02/09   mib     Wrong pointer is freed in mmgsdi_session_build_select_aid
03/09/09   mib     Fixed error conditions during open session
12/01/08   ssr     Handled error condition for some MMGSDI Functions
11/19/08   js      Fixed KW error for null ptr dereferencing
11/05/08   tml     Fixed session release boundary check
09/22/08   js      Fixed Lint Errors
08/28/08   sun     If command fails in the original state, then return original
                   error mmgsdi status to client
06/04/08   tml     Add sw to cnf header, add more debug messages
12/05/07   sun     Mainlined mmgsdi_session_build_status_terminate so that
                   it canbe used for 2G reset as well
11/15/07   sun     Added support for NAA refresh
10/02/07   tml     Added proprietary app session support
09/10/07   tml     Lint changes
08/15/07   jk      Changes to Support DATA Authentication MMGSDI API
05/22/07   nk      Fixed compilation error
05/17/07   wli     Added support for JCDMA RUIM
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
05/02/07   sun     Added support for Service Available and USIM Auth
04/03/07   sp      Send status command before terminating the app
03/19/07   sun     Fixed Lint Error
01/12/07   tml     Allow select AID API for deactivate process default APP
12/12/06   tml     Changed deactivate to use SELECT command to the card with
                   indication for app termination
10/26/06   tml     Decode PIN upon successful Select AID
08/20/06   tml     Added App Reset and 3G Session Reset supports
08/07/06   tml     Added ACL support
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
06/12/06   tml     Externalize mmgsdi_session_release_info.
06/02/06   tml     Added BDN support
05/20/06   tml     Cleanup channel when open failed and lint
04/18/06   nk      Featurized code
12/14/05   tml     MMGSDI Cleanup
11/10/05   tml     Added Lint fixes
11/10/05   tml     Revert Lint fixes
11/10/05   tml     Return client ID as part of the session ID if open and
                   close session successfully
11/09/05   sun     Fixed Lint Errors
11/03/05   tml     Fixed header
09/29/05   tml     Client Deregistration support
07/26/05   sst     Fixed lint errors
07/14/05   tml     Added original request type
07/12/05   tml     Session Management and ISIM support
02/24/05   tml     Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "intconv.h"
#include <stdlib.h>
#include "mmgsdi_uim_uicc.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "mmgsdi_uicc_rsp.h"
#include "mmgsdi_session.h"
#include "mmgsdi_cnf.h"
#include "mmgsdi_refresh.h"
#include "mmgsdi_evt.h"
#include "mmgsdi_gen.h"
#include "mmgsdi_nv.h"
#include "secapi.h"
#include "mmgsdi_multi_profile.h"
#include "mmgsdi_refresh.h"
#include "mmgsdi_refresh_rsp.h"
#ifdef FEATURE_SIMLOCK
#include "mmgsdi_simlock.h"
#endif /* FEATURE_SIMLOCK */

static mmgsdi_return_enum_type mmgsdi_session_add_aid_if_not_present(
  const mmgsdi_aid_type    * aid_ptr,
  mmgsdi_session_info_type * session_info_ptr,
  boolean                  * app_added_to_table_ptr
);

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

#define MMGSDI_SESSION_TYPE_LSB_MASK 0x00000001

/*Max provisioning sessions can be opened by session open ext API*/
#define MMGSDI_SESSION_MAX_PROV_SESSIONS 6

/* ==========================================================================
  FUNCTION mmgsdi_session_start_refresh_auto

  DESCRIPTION
     This function shall start a REFRESH_AUTO request to determine
     if the app deact/switch request can be processed

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
=============================================================================*/
static void mmgsdi_session_start_refresh_auto(
  mmgsdi_request_header_type *request_header_ptr,
  mmgsdi_aid_type            *app_data_ptr,
  mmgsdi_slot_id_enum_type    slot_id,
  int32                       index)
{
  mmgsdi_refresh_req_type refresh_req;

  UIM_MSG_HIGH_0("MMGSDI_SESSION_START_REFRESH_AUTO");

  if((request_header_ptr == NULL) || (app_data_ptr == NULL))
  {
    UIM_MSG_ERR_0("request_header_ptr/app_data_ptr is NULL");
    return;
  }

  /* Create the REFRESH request */
  memset(&refresh_req, 0x00, sizeof(mmgsdi_refresh_req_type));
  mmgsdi_memscpy(&refresh_req.request_header,
                 sizeof(refresh_req.request_header),
                 request_header_ptr,
                 sizeof(mmgsdi_request_header_type));

  refresh_req.request_header.orig_request_type = MMGSDI_REFRESH_REQ;
  refresh_req.request_header.request_type = MMGSDI_REFRESH_REQ;
  refresh_req.request_header.slot_id = slot_id;
  refresh_req.request_header.response_cb = mmgsdi_internal_refresh_app_reset_auto_cb;
  refresh_req.request_header.client_data = (mmgsdi_client_data_type) index;

  refresh_req.refresh_req_type = MMGSDI_REFRESH_ORIG_REFRESH_REQ;
  refresh_req.refresh.card_req.refresh_mode = MMGSDI_REFRESH_APP_RESET_AUTO;
  refresh_req.refresh.card_req.source       = MMGSDI_REFRESH_SOURCE_USER;

  mmgsdi_memscpy((void*)&refresh_req.refresh.card_req.aid,
                 sizeof(refresh_req.refresh.card_req.aid),
                 (void*)app_data_ptr,
                 sizeof(mmgsdi_aid_type));

  /* The CB provided shall be called in case of failures in REFRESH
     handling, so no need to check the return status here */
  (void) mmgsdi_refresh_main(&refresh_req);
}/* mmgsdi_session_start_refresh_auto */


/* ==========================================================================
  FUNCTION MMGSDI_SESSION_INTERNAL_DEACT_CB

  DESCRIPTION
    This function, defined as mmgsdi_callback_type, will handle
    the non-prov application deactivation confirmation generated by MMGSDI

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS          - Command processed,
    MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
    MMGSDI_ERROR            - Other unknown failure

  SIDE EFFECTS
    None
=============================================================================*/
static void mmgsdi_session_internal_deact_cb(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr)
{
  int32                              client_index    = 0;
  mmgsdi_request_header_type         req_header      = {0};
  uim_cmd_type                      *uim_cmd_ptr     = NULL;
  mmgsdi_client_req_extra_info_type *extra_param_ptr = NULL;
  mmgsdi_sw_status_type              status_word     = {0};
  mmgsdi_return_enum_type            mmgsdi_status   = MMGSDI_SUCCESS;

  UIM_MSG_HIGH_0("mmgsdi_session_internal_deact_cb");

  if (cnf_ptr == NULL)
  {
    UIM_MSG_ERR_0("Session CB: Cnf Ptr is NULL");
    return;
  }

  switch(cnf)
  {
    /* Handle the application deactivation confirmation */
    case MMGSDI_SESSION_CLOSE_CNF:
      /* Obtain the index */
      client_index = (int32)cnf_ptr->session_close_cnf.response_header.client_data;

      /* Retrieve the original Session close request */
      mmgsdi_status = mmgsdi_util_get_client_request_table_info(
                                    client_index,
                                    &req_header,
                                    &uim_cmd_ptr,
                                    &extra_param_ptr);

      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_1("Could not extract the client info - 0x%x",mmgsdi_status);
        return;
      }

      if(extra_param_ptr == NULL)
      {
        UIM_MSG_ERR_0("Extra parameters are missing");
        mmgsdi_util_free_client_request_table_index(client_index);
        return;
      }

      /* Copy the original Session close request header */
      mmgsdi_memscpy((void*)&req_header,
                     sizeof(req_header),
                     (void*)&extra_param_ptr->close_data.request_header,
                     sizeof(mmgsdi_request_header_type));

      /* Close the logical channel associated with the application even if
         application termination failed */
      if(status != MMGSDI_SUCCESS)
      {
        UIM_MSG_HIGH_1("App termination failed with status=0x%x.Closing channel",
                       status);
      }
      mmgsdi_status = mmgsdi_uim_uicc_close_channel(&req_header, NULL, FALSE,
                                                    NULL, NULL, MMGSDI_MAX_CHANNELS);

      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_1("Close channel req failed,status=0x%x",mmgsdi_status);
        /* If a confirmation has already been sent for this command as part of
           CARD_ERROR handling, free only the client_req_table index. */
        if((client_index >= 0) && (client_index < MMGSDI_MAX_CLIENT_REQ_INFO) &&
           (mmgsdi_client_req_table_info_ptr[client_index]->uim_rsp_status == MMGSDI_UIM_RSP_NOT_RECEIVED))
        {
          UIM_MSG_HIGH_2("Free client_req_table index 0x%x for cmd 0x%x",
                         client_index,
                         mmgsdi_client_req_table_info_ptr[client_index]->request_type);

          mmgsdi_util_free_client_request_table_index(client_index);
          break;
        }
        /* Queue the confirmation */
        mmgsdi_status = mmgsdi_cnf_build_and_queue(mmgsdi_status,
                          &req_header,
                          NULL,
                          NULL,
                          FALSE,
                          status_word);
      }
      mmgsdi_util_free_client_request_table_index(client_index);
      break;

    default:
      break;
  }
}/*mmgsdi_session_internal_deact_cb*/


/* ==========================================================================
  FUNCTION MMGSDI_SESSION_MISMATCH_DEACTIVATE_CB

  DESCRIPTION
    This function defined as mmgsdi_callback_type, will handle the
    deactivation of source session or destination app first and then
    trigger the activation of the destination app on source session.

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS          - Command processed,
    MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
    MMGSDI_ERROR            - Other unknown failure

  SIDE EFFECTS
    None
=============================================================================*/
static void mmgsdi_session_mismatch_deactivate_cb(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr)
{
  int32                              client_index    = 0;
  mmgsdi_request_header_type         req_header      = {0};
  uim_cmd_type                      *uim_cmd_ptr     = NULL;
  mmgsdi_client_req_extra_info_type *extra_param_ptr = NULL;
  mmgsdi_sw_status_type              status_word     = {0};
  mmgsdi_return_enum_type            mmgsdi_status   = MMGSDI_SUCCESS;
  mmgsdi_session_id_status_enum_type session_status  = MMGSDI_SESSION_ID_UNASSIGNED;

  (void)cnf;

  UIM_MSG_HIGH_0("mmgsdi_session_mismatch_deactivate_cb");

  if (cnf_ptr == NULL)
  {
    return;
  }

  /* Obtain the index of client_req_table_info_ptr for getting the original
     activation req and then trigger the activation after preparing the req  */
  client_index = (int32)cnf_ptr->response_header.client_data;
  if (client_index < 0 || client_index >= MMGSDI_MAX_CLIENT_REQ_INFO)
  {
    return;
  }

  /* Retrieve the original Session switch/deactivate request */
  mmgsdi_status = mmgsdi_util_get_client_request_table_info(
                    client_index,
                    &req_header,
                    &uim_cmd_ptr,
                    &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("mmgsdi session deactivate cb: Could not extract the client info - 0x%x",
                  mmgsdi_status);
    return;
  }

  /* We will definitely have extra_param here because while queuing the
     deactivation req, we stored the orig request data in extra_param and
     allocated a client index in client_req_table_info which has been passed
     as client_data here */
  if (extra_param_ptr != NULL)
  {
    mmgsdi_session_act_or_switch_prov_req_type msg;

    /* If the request is already cancelled and cnf is sent to client then
       no need to process request. Only remove entry from client_req_table */
    if(mmgsdi_client_req_table_info_ptr[client_index] != NULL &&
       mmgsdi_client_req_table_info_ptr[client_index]->uim_rsp_status == MMGSDI_UIM_RSP_NOT_RECEIVED)
    {
      UIM_MSG_ERR_2("Free client_req_table index 0x%x without processing rsp for cmd 0x%x",
                     client_index, mmgsdi_client_req_table_info_ptr[client_index]->request_type);
      mmgsdi_util_free_client_request_table_index(client_index);
      return;
    }

    /* Create the app activate request */
    mmgsdi_memscpy(&msg.request_header,
                   sizeof(msg.request_header),
                   &req_header,
                   sizeof(mmgsdi_request_header_type));
    msg.request_header.request_type = MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ;

    msg.app_id_data = extra_param_ptr->open_data.aid.aid;

    session_status = mmgsdi_util_is_session_id_valid(msg.request_header.session_id);

    do
    {
      mmgsdi_session_type_enum_type req_session_type  = MMGSDI_MAX_SESSION_TYPE_ENUM;
      mmgsdi_technology_enum_type   request_tech      = MMGSDI_TECH_UNKNOWN;
      mmgsdi_attribute_enum_type    request_attr      = MMGSDI_ATTR_UNKNOWN;
      mmgsdi_attribute_enum_type    attr_iterator     = MMGSDI_ATTR_UNKNOWN;

      /* Check for session_status if this has already been activated as part of
         req handling on some other task return Error */
      if (session_status != MMGSDI_SESSION_ID_ASSIGNED_NOT_ACTIVE)
      {
        UIM_MSG_ERR_1("Session: 0x%x requested is already got activated by some other task",
                      msg.request_header.session_id);
        mmgsdi_status = MMGSDI_ERROR;
        break;
      }

      /* If the de-activation failed and requested session_id is NOT_ACTIVE
         it means that the de-activation for freeing the destination APP
         got failed. It is possible by this time destination APP already
         been freed because of activation req on session on which the
         destination app was mapped. We can check if dest APP is free
         then proceed with the activation */
      if (status == MMGSDI_SUCCESS)
      {
        /* Send the activation command for the requested session */
        mmgsdi_status = mmgsdi_session_pre_prov_activation_start(&msg);
        break;
      }

      /* Getting the session type of the req session */
      mmgsdi_status = mmgsdi_util_get_session_type(msg.request_header.session_id,
                                                   &req_session_type, NULL);

      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_1("Unable to get session_type for session_id: 0x%x",
                    msg.request_header.session_id);
        break;
      }

      mmgsdi_status = mmgsdi_util_get_tech_and_attr_from_session_type(req_session_type,
                                                                      &request_tech,
                                                                      &request_attr);
      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        break;
      }

      for (attr_iterator = MMGSDI_ATTR_PRIMARY; attr_iterator <= MMGSDI_ATTR_TERTIARY; attr_iterator++)
      {
        mmgsdi_session_type_enum_type corr_session_type = MMGSDI_MAX_SESSION_TYPE_ENUM;
        mmgsdi_session_id_type        corr_session_id   = MMGSDI_SESSION_ID_ZERO;
        mmgsdi_slot_id_enum_type      slot_id           = MMGSDI_MAX_SLOT_ID_ENUM;
        mmgsdi_channel_info_type     *channel_info_ptr  = NULL;
        mmgsdi_aid_type               app_data          = extra_param_ptr->open_data.aid;

        /* We need to check the APPs mapped on session with same
           technology but different attribute */
        if (attr_iterator == request_attr)
        {
          continue;
        }
        mmgsdi_status = mmgsdi_util_get_session_type_from_tech_and_attr(request_tech,
                                                                        attr_iterator,
                                                                        &corr_session_type);
        if (mmgsdi_status != MMGSDI_SUCCESS)
        {
          break;
        }
        mmgsdi_status = mmgsdi_util_get_mmgsdi_session_id_from_type(corr_session_type,
                                                                    &corr_session_id);
        if (mmgsdi_status != MMGSDI_SUCCESS)
        {
          UIM_MSG_ERR_1("Unable to get session_id for session_type: 0x%x",
                        corr_session_type);
          break;
        }
        mmgsdi_status = mmgsdi_util_get_session_app_info(corr_session_id,
                                                         &slot_id,
                                                         NULL,
                                                         NULL,
                                                         &channel_info_ptr,
                                                         NULL,
                                                         NULL);
        /* It is possible that there is no channel mapped with the
           corresponding sesison. So, this is not the error case */
        if (mmgsdi_status != MMGSDI_SUCCESS || channel_info_ptr == NULL)
        {
          mmgsdi_status = MMGSDI_SUCCESS;
          continue;
        }

        /* Proceed only if the req Slot_id matches with the slot_id on
           which corr_session_id is active. */
        if (slot_id == msg.request_header.slot_id)
        {
          /* It is possible that only channel is opened and SELECT AID
             is still pending for that session. Just continue in that case. */
          if (channel_info_ptr != NULL && channel_info_ptr->app_info_ptr == NULL)
          {
            continue;
          }
          /* If comparison SUCCESS means this is dest app and it is active
             on some other session. Return Error in that case */
          if (mmgsdi_util_cmp_aid(&channel_info_ptr->app_info_ptr->app_data,
                                  &app_data,
                                  FALSE) == MMGSDI_SUCCESS)
          {
            UIM_MSG_ERR_1("Requested app is active on session_type: 0x%x", corr_session_type);
            mmgsdi_status = MMGSDI_ERROR;
            break;
          }
        }
    }

      /* If there is no session on which dest APP is active then proceed
         with the activation of req session */
      if (mmgsdi_status == MMGSDI_SUCCESS)
      {
        mmgsdi_status = mmgsdi_session_pre_prov_activation_start(&msg);
      }
    }while(0);

    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      /* Queue the confirmation */
      mmgsdi_status = mmgsdi_cnf_build_and_queue(mmgsdi_status,
                                                 &req_header,
                                                 NULL,
                                                 NULL,
                                                 FALSE,
                                                 status_word);
    }
  }
  /* Free the index here which we allocated when queuing the de-activation
     request for src session or dest app. If we need an index in
     client_re_table then it will be allocated later in the function.
     This is done to make sure no mem_leak if we will not come to this
     function again. */
  mmgsdi_util_free_client_request_table_index(client_index);
}/* mmgsdi_session_mismatch_deactivate_cb */


/* ==========================================================================
  FUNCTION MMGSDI_SESSION_DEACTIVATE_DUMMY_CB

  DESCRIPTION
    This function defined as dummy mmgsdi_callback_type, will handle the
    deactivation of any mismatched session which is wrongly mapped but
    not tied to the app for which we got the activation req.

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS          - Command processed,
    MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
    MMGSDI_ERROR            - Other unknown failure

  SIDE EFFECTS
    None
=============================================================================*/
static void mmgsdi_session_deactivate_mismatch_dummy_cb(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr)
{
  mmgsdi_session_type_enum_type session_type = MMGSDI_MAX_SESSION_TYPE_ENUM;

  (void)cnf;

  if (cnf_ptr == NULL)
  {
    return;
  }

  (void)mmgsdi_util_get_session_type(cnf_ptr->session_deactivate_cnf.response_header.session_id,
                                     &session_type,
                                     NULL);

  UIM_MSG_MED_2("mmgsdi_session_deactivate_mismatch_dummy_cb for session_type: 0x%x, status: 0x%x",
                session_type, status);
} /* mmgsdi_session_deactivate_dummy_cb */


/* ==========================================================================
  FUNCTION MMGSDI_SESSION_DEACTIVATE_FOUND_MISMATCH

  DESCRIPTION
    This function starts a de-activation procedure for the session which are
    wrongly mapped according to current activation request. If the session is
    tied to the destination app then we require a callback from where
    activation can be triggered again once the de-activation is complete.

    is_dest_app: This input parameter tells us that the session_id received
    for de-activation is tied to the destination app. In that case, after
    trigerring the de-activation wait for the callback to make sure that
    dest app is free. From callback trigger the activation process.

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_session_deactivate_found_mismatch(
  const mmgsdi_session_act_or_switch_prov_req_type  *msg_ptr,
  mmgsdi_session_id_type                             session_id,
  boolean                                            is_dest_app
)
{
  mmgsdi_return_enum_type            mmgsdi_status   = MMGSDI_SUCCESS;
  int32                              index           = 0;
  mmgsdi_client_req_extra_info_type *extra_param_ptr = NULL;
  mmgsdi_session_type_enum_type      session_type    = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_slot_id_enum_type           slot_id         = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_task_cmd_type              *task_cmd_ptr    = NULL;
  mmgsdi_session_deact_req_type     *deact_msg_ptr   = NULL;
  int32                              total_mem_len   = 0;
  uint32                             task_cmd_len    = 0;
  mmgsdi_task_enum_type              mmgsdi_task     = MMGSDI_TASK_MAX;

  MMGSDIUTIL_RETURN_IF_NULL(msg_ptr);

  mmgsdi_status = mmgsdi_util_get_session_type(session_id, &session_type, NULL);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_get_session_app_info(session_id,
                                                   &slot_id,
                                                   NULL, NULL, NULL, NULL, NULL);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  UIM_MSG_HIGH_2("mmgsdi_session_deactivate_found_mismatch: Deactivating session type: 0x%x on slot: 0x%x",
                 session_type, slot_id);

  total_mem_len += uint32toint32(sizeof(mmgsdi_task_cmd_type));

  task_cmd_len = int32touint32(total_mem_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  deact_msg_ptr = &task_cmd_ptr->cmd.cmd.session_deact_req;

  /* If the session_id is tied to destination app then save the actual
     activation parameters to use them later from the callback fn to
     trigger the activation once the dest app is freed otherwise give
     the dummy callback because there is no need to wait for de-activation
     of session which is not tied to dest app.  */
  if(is_dest_app)
  {
    mmgsdi_session_open_extra_info_type extra_param;
    memset((void*)&extra_param, 0x00, sizeof(mmgsdi_session_open_extra_info_type));

    extra_param.aid.aid = msg_ptr->app_id_data;

    mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
                      MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ,
                      (void*)&extra_param, &extra_param_ptr);

    if (mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
    {
      UIM_MSG_ERR_0("Could not allocate memory for extra_param");
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(task_cmd_ptr);
      return mmgsdi_status;
    }

    mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(task_cmd_ptr);
      return mmgsdi_status;
    }

    mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                      index,
                      &msg_ptr->request_header,
                      NULL,
                      extra_param_ptr);

    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      mmgsdi_util_free_client_request_table_index(index);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(task_cmd_ptr);
      return mmgsdi_status;
    }

    deact_msg_ptr->request_header.client_data  = (mmgsdi_client_data_type)index;
    deact_msg_ptr->request_header.response_cb  = mmgsdi_session_mismatch_deactivate_cb;
  }
  else
  {
    deact_msg_ptr->request_header.response_cb  = mmgsdi_session_deactivate_mismatch_dummy_cb;
    deact_msg_ptr->request_header.client_data  = 0;
  }

  /*---------------------------------------------------------------------------
    Populating mmgsdi_session_deact_req_type header
      1) Session ID
      2) Request TYPE
  ---------------------------------------------------------------------------*/

  task_cmd_ptr->cmd.cmd_enum                      = MMGSDI_SESSION_DEACTIVATE_REQ;
  deact_msg_ptr->internal_deact_req               = TRUE;
  deact_msg_ptr->in_priority_queue                = TRUE;
  deact_msg_ptr->is_reason_mismatch               = TRUE;
  deact_msg_ptr->request_header.session_id        = session_id;
  deact_msg_ptr->request_header.slot_id           = slot_id;
  deact_msg_ptr->request_header.request_type      = MMGSDI_SESSION_DEACTIVATE_REQ;
  deact_msg_ptr->request_header.orig_request_type = MMGSDI_SESSION_DEACTIVATE_REQ;

  /*---------------------------------------------------------------------------
   Calculate the payload len and request len
  ---------------------------------------------------------------------------*/
  deact_msg_ptr->request_header.request_len = total_mem_len;
  deact_msg_ptr->request_header.payload_len =
    total_mem_len - uint32toint32(sizeof(mmgsdi_request_header_type));

  mmgsdi_task = mmgsdi_util_get_task_from_slot(slot_id);

  /* Queue the command */
  mmgsdi_status = mmgsdi_cmd_ext(task_cmd_ptr, mmgsdi_task);

  UIM_MSG_HIGH_2("Queue of mmgsdi_session_deact_req_type cmd for mismatch, status: 0x%x in task: 0x%x",
                 mmgsdi_status, mmgsdi_task);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put
       onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    if(is_dest_app)
    {
      mmgsdi_util_free_client_request_table_index(index);
    }
  }
  return mmgsdi_status;
} /* mmgsdi_session_deactivate_found_mismatch */


/* ==========================================================================
  FUNCTION MMGSDI_SESSION_CLOSE_CHANNEL_INTERNAL_CB

  DESCRIPTION
    This function is a dummy callback used after close channel has been
    completed and no external client needs to be notified

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
=============================================================================*/
static void mmgsdi_session_close_channel_internal_cb(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr)
{
  (void)status;
  (void)cnf;
  (void)cnf_ptr;

  UIM_MSG_LOW_0("mmgsdi_session_close_channel_internal_cb");
  /* External client has already been notified via a confirmation. This
     is simply a dummy to ensure close channel request can end gracefully
     with the invocation of an internal callback */
}/*mmgsdi_session_close_channel_internal_cb*/


/* ==========================================================================
  FUNCTION MMGSDI_SESSION_BUILD_UIM_OPEN_CHANNEL_RSP

  DESCRIPTION
    This function will create a uim response for MANAGE CHANNEL command
    and will put the response in mmgsdi_uim_report() which is mmgsdi's
    calback function for UIM

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    mmgsdi_return_enum_type

  SIDE EFFECTS
    Memory allocated to extra_param_ptr needs to be free'ed by
    mmgsdi response processing
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_session_build_uim_open_channel_rsp(
  byte                                channel_id,
  mmgsdi_request_header_type          request_header,
  mmgsdi_aid_type                    *aid_ptr,
  uim_rpt_status                      uim_status,
  boolean                             select_rsp_requested
)
{
  int32                                index             = 0;
  mmgsdi_return_enum_type              mmgsdi_status     = MMGSDI_SUCCESS;
  mmgsdi_client_req_extra_info_type   *extra_param_ptr   = NULL;
  uim_rpt_type                         uim_rpt;
  mmgsdi_session_open_extra_info_type  extra_info;

  memset(&uim_rpt, 0x00, sizeof(uim_rpt_type));
  memset(&extra_info, 0x00, sizeof(mmgsdi_session_open_extra_info_type));

  UIM_MSG_HIGH_1("mmgsdi_session_build_uim_open_channel_rsp with uim_status:0x%x",
                 uim_status);

  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
      Allocate and Populate Extra Parameter
      - Free Request Table if error in Extra Parameter
      - Free temp data holder if error
    ---------------------------------------------------------------------------*/
  if(aid_ptr != NULL || select_rsp_requested)
  {
    if(aid_ptr != NULL)
    {
      mmgsdi_memscpy(&extra_info.aid,
                     sizeof(extra_info.aid),
                     aid_ptr,
                     sizeof(mmgsdi_aid_type));
    }

    extra_info.select_rsp_requested = select_rsp_requested;

    if(!select_rsp_requested)
    {
      mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
                        MMGSDI_SESSION_OPEN_REQ,
                        (void*)&extra_info, &extra_param_ptr);
    }
    else
    {
      mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
                        MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_REQ,
                        (void*)&extra_info, &extra_param_ptr);
    }

    if (mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
    {
      /*---------------------------------------------------------------------------
        1) Release the client table
        2) Release temp data  (Since client table has not been populated yet)
      ---------------------------------------------------------------------------*/
      mmgsdi_util_free_client_request_table_index(index);
      return mmgsdi_status;
    }
  }
  /* building an Open response, the request_type has to be set to OPEN */
  if(!select_rsp_requested)
  {
    request_header.request_type = MMGSDI_SESSION_OPEN_REQ;
  }
  else
  {
    request_header.request_type = MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_REQ;
  }
  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                    index, &request_header, NULL, extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Unable to put in the request table, free necessary memory locations */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }

  if(uim_status == UIM_PASS)
  {
    uim_rpt.sw1                   = SW1_NORMAL_END;
    uim_rpt.sw2                   = SW2_NORMAL_END;
  }
  else
  {
    /* The status word is not coming from the card.  Return
    a generic error rather a card specific error that
    could be confused for an actual response from the card */
    uim_rpt.sw1                   = SW1_NO_STATUS;
    uim_rpt.sw2                   = SW2_NORMAL_END;
  }
  uim_rpt.rpt_status              = uim_status;
  uim_rpt.rpt_type                = UIM_MANAGE_CHANNEL_R;
  uim_rpt.rpt.channel.channel_num = channel_id;
  uim_rpt.cmd_transacted          = FALSE;
  uim_rpt.user_data               = int32touint32(index);

  /* Fake a uim response */
  mmgsdi_uim_report(&uim_rpt);

  return mmgsdi_status;
} /* mmgsdi_session_build_uim_open_channel_rsp */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_POPULATE_SESSION_INFO_ACT_PROV

   DESCRIPTION:
     This function will populate the session information into the
     client registration table while activating provsioning session.

   DEPENDENCIES:


   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:
     None.

   CRITICAL SECTIONS:
     The function uses mmgsdi_client_app_data_crit_sect_ptr to protect accesses
     to session info ptr and channel info ptr.
-----------------------------------------------------------------------------*/
static mmgsdi_return_enum_type mmgsdi_session_populate_session_info_act_prov(
  const mmgsdi_session_info_type           *session_info_ptr)
{
  uint32                     i                  = 0;
  uint32                     j                  = 0;
  mmgsdi_slot_data_type    * slot_data_ptr      = NULL;
  uint8                      channel_info_index = MMGSDI_MAX_CHANNEL_INFO;

  MMGSDIUTIL_RETURN_IF_NULL(session_info_ptr);

  MMGSDIUTIL_CHECK_CLIENT_APP_DATA_CRIT_SECT_ACCESS;

  /* Set application status */
  mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->app_selected = TRUE;

  mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->app_state =
    MMGSDI_APP_STATE_ACTIVATED;

  /* Get the slot data ptr */
  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(
                    mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->slot_id);
  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  for(i=0; i<MMGSDI_MAX_CLIENT_INFO; i++)
  {
    if (mmgsdi_client_id_reg_table[i] == NULL)
    {
      continue;
    }

    for(j=0; j<MMGSDI_MAX_SESSION_INFO; j++)
    {
      if (mmgsdi_client_id_reg_table[i]->session_info_ptr[j] == NULL)
      {
        continue;
      }

      channel_info_index = mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->channel_info_index;
      if((session_info_ptr->channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
         (channel_info_index != session_info_ptr->channel_info_index))
      {

        if(/* Update channel info for sessions of provisioning type */
           (mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->session_type ==
              session_info_ptr->session_type)
                                             ||
            /* Update channel info for sessions of card slot1 type */
           ((mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->slot_id == MMGSDI_SLOT_1) &&
            (mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->session_type ==
              MMGSDI_CARD_SESSION_SLOT_1) &&
            (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->channel_id ==
              MMGSDI_DEFAULT_CHANNEL))
                                             ||
            /* Update channel info for sessions of card slot2 type */
           ((mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->slot_id == MMGSDI_SLOT_2) &&
            (mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->session_type ==
              MMGSDI_CARD_SESSION_SLOT_2) &&
            (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->channel_id ==
              MMGSDI_DEFAULT_CHANNEL))
                                             ||
            /* Update channel info for sessions of card slot3 type */
           ((mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->slot_id == MMGSDI_SLOT_3) &&
            (mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->session_type ==
              MMGSDI_CARD_SESSION_SLOT_3) &&
            (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->channel_id ==
              MMGSDI_DEFAULT_CHANNEL)))
        {
          UIM_MSG_HIGH_1("Channel mapping updated for session_id=0x%x",
                         mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->session_id);

          if(( channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
             ( mmgsdi_channel_info_ptr_table[channel_info_index] != NULL) &&
             ( mmgsdi_channel_info_ptr_table[channel_info_index]->num_connections > 0))
          {
            mmgsdi_channel_info_ptr_table[channel_info_index]->num_connections--;
          }
          mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->num_connections++;
          mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->channel_info_index =
            session_info_ptr->channel_info_index;
        }
      }
    } /* for j<MAX_SESSION_INFO */
  } /* for i<MAX_CLIENT_INFO */

  mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->channel_state =
    MMGSDI_CHANNEL_STATE_NONE;
  mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->select_rsp_type =
    MMGSDI_SELECT_RSP_FCP;

  return MMGSDI_SUCCESS;
} /* mmgsdi_session_populate_session_info_act_prov */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_POPULATE_SESSION_INFO

   DESCRIPTION:
     This function will populate the session information into the
     client registration table.

   DEPENDENCIES:


   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:
     None.

   CRITICAL SECTIONS:
     The function uses mmgsdi_client_app_data_crit_sect_ptr to protect accesses
     to session info ptr and channel info ptr.
-----------------------------------------------------------------------------*/
static mmgsdi_return_enum_type mmgsdi_session_populate_session_info(
  const mmgsdi_uim_report_rsp_type           *uim_rsp_ptr,
  const mmgsdi_request_header_type           *req_header_ptr,
  const mmgsdi_get_file_info_extra_info_type *extra_info_ptr)
{
  mmgsdi_session_info_type * session_info_ptr   = NULL;
  mmgsdi_return_enum_type    mmgsdi_status      = MMGSDI_ERROR;

  MMGSDIUTIL_RETURN_IF_NULL_2(uim_rsp_ptr, req_header_ptr);

  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  do
  {
    mmgsdi_status = mmgsdi_util_get_session_app_info(req_header_ptr->session_id,
                                                     NULL,
                                                     NULL,
                                                     NULL,
                                                     NULL,
                                                     &session_info_ptr,
                                                     NULL);

    if(mmgsdi_status != MMGSDI_SUCCESS ||
       session_info_ptr == NULL ||
       session_info_ptr->channel_info_index >= MMGSDI_MAX_CHANNEL_INFO ||
       mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index] == NULL ||
       mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr == NULL)
    {
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    /* If session type is non prov type then set the app state to ready
       since no other action will be taken on these apps */
    if(((req_header_ptr->orig_request_type == MMGSDI_SESSION_OPEN_REQ) ||
        (req_header_ptr->orig_request_type == MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_REQ)) &&
        (mmgsdi_util_is_non_prov_session(session_info_ptr->session_type)))
    {
      /* If app is already selected leave the state as it means a session
         is already open to this app and the state will get updated based
         on the circumstances of that session. Only if the app is not selected
         update the state */
      if (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->app_selected
            == FALSE)
      {
        mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->app_selected =
          TRUE;
        mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->app_state =
          MMGSDI_APP_STATE_READY;
      }
      else
      {
        UIM_MSG_HIGH_1("App is already selected.App state=0x%x",
                       mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->app_state);
      }
    }

    /* If session type is GW or 1X prov type then move the sessions
       with same session type to this channel */
    if((req_header_ptr->orig_request_type == MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ) &&
       (mmgsdi_util_is_prov_session(session_info_ptr->session_type)))
    {
      (void) mmgsdi_session_populate_session_info_act_prov(session_info_ptr);
    } /* if(session_type == GW/1X/PRI/SEC/PROV && req == ACT_SW_PROV)*/

    /*Copy select response type in app info*/
    if(extra_info_ptr != NULL)
    {
      mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->select_rsp_type =
        extra_info_ptr->select_rsp_type;
    }
    else
    {
      mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->select_rsp_type =
        MMGSDI_SELECT_RSP_FCP;
    }

    mmgsdi_status =  mmgsdi_uicc_rsp_decode_pin_header_data (
        req_header_ptr,
        extra_info_ptr,
        uim_rsp_ptr->rsp_data.data_ptr,
        uim_rsp_ptr->rsp_data.data_len);

  }while(0);

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  return mmgsdi_status;
} /* mmgsdi_session_populate_session_info */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_RELEASE_APP_INFO

   DESCRIPTION:
     This function will release application information in regard to the
     session where the specific application has been deactivated.
     The session info, e.g., the session id, channel id and slot id will
     remain valid since this function does not imply a channel has been
     closed.

   DEPENDENCIES:


   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:
     NONE

   CRITICAL SECTION:
     The calling function must enter mmgsdi_client_app_data_crit_sect_ptr crit
     sect before invoking this function.
     Protection is for session, app, pin and channel data.

-----------------------------------------------------------------------------*/
static mmgsdi_return_enum_type  mmgsdi_session_release_app_info (
 mmgsdi_session_info_type  *session_info_ptr
)
{
  int32                      c_index             = 0;
  int32                      s_index             = 0;
  mmgsdi_session_info_type  *curr_session_ptr    = NULL;
  boolean                    session_match_found = FALSE;
  mmgsdi_int_app_info_type  *remove_app_info_ptr = NULL;
  uint8                      app_index           = 0;
  uint8                      slot_index          = MMGSDI_SLOT_1_INDEX;
  mmgsdi_return_enum_type    mmgsdi_status       = MMGSDI_SUCCESS;
  uint8                      channel_index       = 0;

  UIM_MSG_HIGH_0("MMGSDI_SESSION_RELEASE_APP_INFO");

  MMGSDIUTIL_RETURN_IF_NULL(session_info_ptr);

  if((session_info_ptr->channel_info_index >= MMGSDI_MAX_CHANNEL_INFO) ||
     (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index] == NULL) ||
     (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr == NULL))
  {
    UIM_MSG_ERR_2("Null Ptr session_id=0x%x, chnl_ptr=0x%x",
              session_info_ptr->session_id,
              mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]);
    return MMGSDI_ERROR;
  }

  if (mmgsdi_util_is_prov_session(session_info_ptr->session_type))
  {
    if(mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->num_connections == 0)
    {
      mmgsdi_util_reset_app_info(session_info_ptr->channel_info_index);
    }
  }
  else if (mmgsdi_util_is_non_prov_session(session_info_ptr->session_type) ||
           mmgsdi_util_is_card_session(session_info_ptr->session_type))
  {
    if(mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->num_connections == 0)
    {
      /* Remove the app from the channel_info_table then
         scan through all open sessions to identify the ones linked to the
         same app. If other sessions are linked to same app break out -
         don't clear app data at this time */
      remove_app_info_ptr = mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr;
      mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr = NULL;
      mmgsdi_status = mmgsdi_util_get_slot_index(
                        mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->slot_id,
                        &slot_index);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        return MMGSDI_ERROR;
      }

      for (c_index = 0;c_index < MMGSDI_MAX_CLIENT_INFO;c_index++)
      {
        if (mmgsdi_client_id_reg_table[c_index] == NULL)
        {
          continue;
        }

        for(s_index = 0;s_index < MMGSDI_MAX_SESSION_INFO;s_index++)
        {
          curr_session_ptr =
          mmgsdi_client_id_reg_table[c_index]->session_info_ptr[s_index];
          if((curr_session_ptr == NULL) ||
             (curr_session_ptr == session_info_ptr) ||
             (curr_session_ptr->channel_info_index >= MMGSDI_MAX_CHANNEL_INFO) ||
             (mmgsdi_channel_info_ptr_table[curr_session_ptr->channel_info_index] == NULL) ||
             (mmgsdi_channel_info_ptr_table[curr_session_ptr->channel_info_index]->app_info_ptr == NULL) ||
             (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->slot_id !=
                mmgsdi_channel_info_ptr_table[curr_session_ptr->channel_info_index]->slot_id))
          {
            continue;
          }

          if (mmgsdi_util_cmp_aid(
                &mmgsdi_channel_info_ptr_table[curr_session_ptr->channel_info_index]->app_info_ptr->app_data,
                &remove_app_info_ptr->app_data,
                FALSE)
               == MMGSDI_SUCCESS)
          {
            session_match_found = TRUE;
            break;
          }
        }
        if(session_match_found)
        {
          break;
        }
      }

      /* If no other session is associated with the app then free app info */
      if(session_match_found)
      {
        return MMGSDI_SUCCESS;
      }

      /* for the apps that are skipped termination do not have channel entry in session info table
         so check on the channel table also should be done before resetting the app info ptr */
      for (channel_index = 0; channel_index < MMGSDI_MAX_CHANNEL_INFO; channel_index++)
      {
        if ((mmgsdi_channel_info_ptr_table[channel_index] == NULL)||
            (channel_index == session_info_ptr->channel_info_index) ||
            (mmgsdi_channel_info_ptr_table[channel_index]->slot_id !=
               mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->slot_id) ||
            (mmgsdi_channel_info_ptr_table[channel_index]->app_info_ptr == NULL))
        {
          continue;
        }

        if (mmgsdi_util_cmp_aid(
              &mmgsdi_channel_info_ptr_table[channel_index]->app_info_ptr->app_data,
              &remove_app_info_ptr->app_data,
              FALSE)
             == MMGSDI_SUCCESS)
        {
          session_match_found = TRUE;
          break;
        }
      }

      /* If no other channel is associated with the app then free app info */
      if(session_match_found)
      {
        return MMGSDI_SUCCESS;
      }

      mmgsdi_status = mmgsdi_util_get_app_index_for_app(&remove_app_info_ptr->app_data,
                                                        mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->slot_id,
                                                        &app_index);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_0("App not found in the app table");
        return MMGSDI_SUCCESS;
      }

      /* Cleanup PIN info connected to the app */
      mmgsdi_util_app_pin_cleanup(remove_app_info_ptr,
                                  mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->slot_id);
      /* EONS Info Cleanup */
      mmgsdi_util_app_eons_cleanup(remove_app_info_ptr);

      (void)mmgsdi_util_clear_app_capabilities(&remove_app_info_ptr->app_capabilities);

      /* Free the memory for the app if the app is not in EF DIR */
      if(remove_app_info_ptr->app_not_in_efdir == TRUE)
      {
        MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_app_pin_table_ptr[slot_index][app_index]);
      }
      else
      {
        if(remove_app_info_ptr->app_state > MMGSDI_APP_STATE_UNKNOWN)
        {
          remove_app_info_ptr->app_state = MMGSDI_APP_STATE_DETECTED;
        }

        remove_app_info_ptr->perso_state = MMGSDI_SIMLOCK_NOT_VERIFIED;
        remove_app_info_ptr->app_selected = FALSE;
        remove_app_info_ptr->perso_info.feature = MMGSDI_MAX_PERSO_FEATURE_ENUM;
        remove_app_info_ptr->perso_info.num_retries         = 0;
        remove_app_info_ptr->perso_info.num_unblock_retries = 0;
        remove_app_info_ptr->status_app_is_initialized = FALSE;

        memset(&remove_app_info_ptr->aid_in_fcp, 0x00, sizeof(mmgsdi_static_data_type));
      }
    }
  }
  return MMGSDI_SUCCESS;
}/* mmgsdi_session_release_app_info */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_RELEASE_INFO

   DESCRIPTION:
     This function will release information in regard to the session that
     has been deactivated and closed.

   DEPENDENCIES:


   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                              but the service table indicates the card does not
                              have the support

   SIDE EFFECTS:
     The function will free the memory allocated to the input pointer
     before return under all cases.

   CRITICAL SECTIONS:
     The function uses mmgsdi_client_app_data_crit_sect_ptr to protect accesses
     to session info ptr and channel info ptr.
-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_session_release_info (
  mmgsdi_session_info_type *session_info_ptr,
  mmgsdi_cmd_enum_type      orig_request_type
)
{
  int32                     client_index      = 0;
  int32                     session_index     = 0;
  mmgsdi_return_enum_type   mmgsdi_status     = MMGSDI_SUCCESS;
  mmgsdi_slot_data_type    *slot_data_ptr     = NULL;

  MMGSDIUTIL_RETURN_IF_NULL(session_info_ptr);

  UIM_MSG_HIGH_1("MMGSDI_SESSION_RELEASE_INFO session_id=0x%x",
                 session_info_ptr->session_id);

  if((orig_request_type != MMGSDI_SESSION_OPEN_REQ) &&
     (orig_request_type != MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_REQ) &&
     (orig_request_type != MMGSDI_SESSION_OPEN_WITH_MF_REQ) &&
     (orig_request_type != MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ) &&
     (orig_request_type != MMGSDI_SESSION_DEACTIVATE_REQ) &&
     (orig_request_type != MMGSDI_SESSION_CLOSE_REQ) )
  {
    UIM_MSG_ERR_1("session_release_info - invalid orig request type 0x%x",
                  orig_request_type);
    MMGSDIUTIL_TMC_MEM_FREE(session_info_ptr);
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  do
  {
    /* Check valid session id */
    mmgsdi_status = mmgsdi_util_get_client_and_session_index(
                      session_info_ptr->session_id,
                      &client_index,
                      &session_index);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      MMGSDIUTIL_TMC_MEM_FREE(session_info_ptr);
      break;
    }

    if((session_info_ptr->channel_info_index >= MMGSDI_MAX_CHANNEL_INFO) ||
       (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index] == NULL))
    {
      if((orig_request_type == MMGSDI_SESSION_OPEN_REQ)||
         (orig_request_type == MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_REQ)||
         (orig_request_type == MMGSDI_SESSION_OPEN_WITH_MF_REQ) ||
         (orig_request_type == MMGSDI_SESSION_CLOSE_REQ))
      {
        mmgsdi_status = MMGSDI_SUCCESS;
      }
      else
      {
        UIM_MSG_ERR_1("channel info invalid for session_id=0x%x",
                      session_info_ptr->session_id);
        mmgsdi_status = MMGSDI_ERROR;
      }

      /* Free session info ptr when original req is either SESSION OPEN,
         SESSION OPEN WITH MF, SESSION OPEN WITH SELECT RSP or SESSION CLOSE.
         For other req types, free session info ptr only when session is non prov
         session */
      if(mmgsdi_status == MMGSDI_SUCCESS ||
         (mmgsdi_status == MMGSDI_ERROR &&
         (mmgsdi_util_is_non_prov_session(session_info_ptr->session_type))))
      {
        /* Free the memory allocated for REFRESH file registration */
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(session_info_ptr->refresh_data.refresh_files.file_list_ptr);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(session_info_ptr->refresh_data.refresh_files.file_path_ptr);

        /* Free the memory allocated for file update registration */
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(session_info_ptr->file_data.file_list_ptr);

        /* Free the memory allocated for the session */
        MMGSDIUTIL_TMC_MEM_FREE(session_info_ptr);

        /* client_index has not changed since call to mmgsdi_util_get_client_and_session_index */
        mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index] = NULL;
        mmgsdi_client_id_reg_table[client_index]->num_session--;
      }
      break;
    }

    if(mmgsdi_util_is_prov_session(session_info_ptr->session_type))
    {
      /* Dis-associate channel from session */
      if(mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->num_connections > 1)
      {
        mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->num_connections--;
      }
      else if(mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->num_connections == 1 )
      {
        /* channel data should be reset though it is freed below, so that
           mmgsdi_session_release_app_info is processed correctly */
        mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->num_connections
          = 0;

       if(mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr)
        {
          /* Reset the flag so that any subsequent app activations can proceed normally */
          mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->deactivation_in_progress =
            FALSE;
        }

        /* Reset channel and app data only if the channel was actually closed.
           After USIM/CSIM app deactivation by a client, we keep the channel
           open and need to retain the app/channel info so that the app can be
           moved to ACTIVATED state when an ACT_OR_SWITCH_PROV request is received. */
        if (!mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_termination_skipped)
        {
          mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->channel_state
            = MMGSDI_CHANNEL_STATE_NONE;
          mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->select_rsp_type
            = MMGSDI_SELECT_RSP_NO_DATA;

          mmgsdi_status = mmgsdi_session_release_app_info(session_info_ptr);

          /* This channel is now available to be reused.  Get the
             slot_ptr to add it to the available channel count */
          slot_data_ptr = mmgsdi_util_get_slot_data_ptr(
                            mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->slot_id);
          if(slot_data_ptr != NULL)
          {
            slot_data_ptr->available_logical_channels++;
          }

          MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]);
        }
        else if(mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr != NULL)
        {
          if(mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->app_state >
             MMGSDI_APP_STATE_UNKNOWN)
          {
            mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->app_state =
              MMGSDI_APP_STATE_DETECTED;
          }

          mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->perso_state =
            MMGSDI_SIMLOCK_NOT_VERIFIED;
          mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->selective_act_after_app_term_skipped =
            FALSE;
          mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->pre_perso_files_cached =
            FALSE;

          mmgsdi_util_app_eons_cleanup(mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr);
        }
      }
      session_info_ptr->channel_info_index = MMGSDI_MAX_CHANNEL_INFO;

      /* If the request is to close the Prov. session, perform cleanup */
      if(orig_request_type == MMGSDI_SESSION_CLOSE_REQ)
      {
        /* Free the memory allocated for REFRESH file registration */
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(session_info_ptr->refresh_data.refresh_files.file_list_ptr);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(session_info_ptr->refresh_data.refresh_files.file_path_ptr);

        /* Free the memory allocated for file update registration */
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(session_info_ptr->file_data.file_list_ptr);

        /* Free the memory allocated for the session */
        MMGSDIUTIL_TMC_MEM_FREE(session_info_ptr);

        /* client_index has not changed since call to mmgsdi_util_get_client_and_session_index */
        mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index] = NULL;
        mmgsdi_client_id_reg_table[client_index]->num_session--;
      }
      break;
    }

    if(mmgsdi_util_is_non_prov_session(session_info_ptr->session_type) ||
       mmgsdi_util_is_card_session(session_info_ptr->session_type))
    {
      if(mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->num_connections > 1)
      {
        mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->num_connections--;
      }
      else if(mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->num_connections == 1 )
      {
        /* channel data should be reset though it is freed below, so that
           mmgsdi_session_release_app_info is processed correctly */
        mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->num_connections
          = 0;

        /* Reset channel and app data only if the channel was actually closed.
           After USIM/CSIM app deactivation by a client, we keep the channel
           open and need to retain the app/channel info so that the app can be
           moved to ACTIVATED state when an ACT_OR_SWITCH_PROV request is received. */
        if (!mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_termination_skipped)
        {
          mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->channel_state
            = MMGSDI_CHANNEL_STATE_NONE;
          mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->select_rsp_type
            = MMGSDI_SELECT_RSP_NO_DATA;

          /* This channel is now available to be reused.  Get the
             slot_ptr to add it to the available channel count */
          slot_data_ptr = mmgsdi_util_get_slot_data_ptr(
                            mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->slot_id);
          if(slot_data_ptr != NULL)
          {
            slot_data_ptr->available_logical_channels++;
          }

          /* Dis-associate application from session */
          if(mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr != NULL)
          {
            mmgsdi_status = mmgsdi_session_release_app_info(session_info_ptr);
            if (mmgsdi_status != MMGSDI_SUCCESS)
            {
              /* Free the memory allocated for REFRESH file registration */
              MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(session_info_ptr->refresh_data.refresh_files.file_list_ptr);
              MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(session_info_ptr->refresh_data.refresh_files.file_path_ptr);

              /* Free the memory allocated for file update registration */
              MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(session_info_ptr->file_data.file_list_ptr);

              MMGSDIUTIL_TMC_MEM_FREE(
                mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]);

              /* Free the memory allocated for the session */
              MMGSDIUTIL_TMC_MEM_FREE(session_info_ptr);

              /* client_index has not changed since call to mmgsdi_util_get_client_and_session_index */
              mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index] = NULL;
              mmgsdi_client_id_reg_table[client_index]->num_session--;
              break;
            }
          }

          MMGSDIUTIL_TMC_MEM_FREE(
            mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]);
        }
        else if(mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr != NULL)
        {
          if(mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->app_state >
             MMGSDI_APP_STATE_UNKNOWN)
          {
            mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->app_state =
              MMGSDI_APP_STATE_DETECTED;
          }
          mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->perso_state =
            MMGSDI_SIMLOCK_NOT_VERIFIED;
          mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->selective_act_after_app_term_skipped =
            FALSE;

          mmgsdi_util_app_eons_cleanup(mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr);
        }
      }
      session_info_ptr->channel_info_index = MMGSDI_MAX_CHANNEL_INFO;

      /* Dis-associate session from client for Non-prov Sessions/for Card
         Sessions (if a Session Close is explicitly requested) */
      if(mmgsdi_util_is_non_prov_session(session_info_ptr->session_type) ||
         (orig_request_type == MMGSDI_SESSION_CLOSE_REQ))
      {
        /* Free the memory allocated for REFRESH file registration */
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(session_info_ptr->refresh_data.refresh_files.file_list_ptr);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(session_info_ptr->refresh_data.refresh_files.file_path_ptr);

        /* Free the memory allocated for file update registration */
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(session_info_ptr->file_data.file_list_ptr);

        /* Free the memory allocated for the session */
        MMGSDIUTIL_TMC_MEM_FREE(session_info_ptr);

        /* client_index has not changed since call to mmgsdi_util_get_client_and_session_index */
        mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index] = NULL;
        mmgsdi_client_id_reg_table[client_index]->num_session--;
      }
    }
    mmgsdi_status = MMGSDI_SUCCESS;
  }while(0);

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;
  return mmgsdi_status;
}/*mmgsdi_session_release_info*/


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_ACTIVATE_APP_INFO

   DESCRIPTION:
     This function will add the app information to the session index of the
     client. Will be used by open session and refresh

   DEPENDENCIES:


   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                              but the service table indicates the card does not
                              have the support

   SIDE EFFECTS:
    None.

   CRITICAL SECTIONS:
     The function uses mmgsdi_client_app_data_crit_sect_ptr to protect accesses
     to session info ptr.
-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_session_activate_app_info(
  mmgsdi_session_id_type        session_id,
  mmgsdi_static_data_type       app_id
)
{
  int32                      i                   = 0;
  mmgsdi_session_info_type  *session_info_ptr    = NULL;
  mmgsdi_int_app_info_type **app_info_tbl_pptr   = NULL;
  mmgsdi_return_enum_type    mmgsdi_status       = MMGSDI_ERROR;
   mmgsdi_aid_type           aid;

  memset(&aid,0x00, sizeof(mmgsdi_aid_type));

  /* Check if the client table's app info is valid or not, if null, populate
     with the requested data */
  mmgsdi_status = mmgsdi_util_get_session_app_info(session_id,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   &session_info_ptr,
                                                   NULL);

  if(mmgsdi_status != MMGSDI_SUCCESS ||
     session_info_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  app_info_tbl_pptr = mmgsdi_util_get_app_pin_info_tbl_ptr(
                        mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->slot_id);

  if(app_info_tbl_pptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  if(mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr == NULL)
  {
    aid.aid.data_len = app_id.data_len;
    if((app_id.data_len <= MMGSDI_MAX_APP_ID_LEN) &&
       (app_id.data_len > 0))
    {
      mmgsdi_memscpy((void*)aid.aid.data_ptr,
                     sizeof(aid.aid.data_ptr),
                     (void*)app_id.data_ptr,
                     int32touint32(aid.aid.data_len));
    }

    /* Get the AID type from the AID */
    aid.app_type =
      mmgsdi_util_determine_app_type_from_aid(&aid.aid);

    for (i=0; i<MMGSDI_MAX_EXT_APP_INFO; i++)
    {
      if(app_info_tbl_pptr[i] == NULL)
      {
        continue;
      }
      mmgsdi_status = mmgsdi_util_cmp_aid(&app_info_tbl_pptr[i]->app_data,
                                          &aid,
                                          FALSE);

      if (mmgsdi_status == MMGSDI_SUCCESS)
        break;
    }
    if (i < MMGSDI_MAX_EXT_APP_INFO)
    {
      MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;
      mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr =
        app_info_tbl_pptr[i];
      MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;
    }
    else
    {
      UIM_MSG_ERR_1("Reached max apps supported for session 0x%x",
                    session_id);
      return MMGSDI_ERROR;
    }
  }
  else
  {
    UIM_MSG_ERR_0("Invalid Session ID, cannot activate");
    return MMGSDI_ERROR;
  }
  return MMGSDI_SUCCESS;
}/*mmgsdi_session_activate_app_info*/


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_UICC_OPEN_CHANNEL_CHK

   DESCRIPTION:
     This helper function gets called as a part of session activate/switch
     provision app processing. The function decides if we need to open a
     channel with card and also sends a open channel command to the card.

   DEPENDENCIES:
    Only valid for UICC cards. Request Input Param should contain valid AID
    data

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_session_uicc_open_channel_chk(
  mmgsdi_session_act_or_switch_prov_req_type *req_ptr
)
{
  mmgsdi_session_info_type *session_info_ptr              = NULL;
  mmgsdi_return_enum_type   mmgsdi_status                 = MMGSDI_ERROR;
  mmgsdi_slot_data_type    *slot_data_ptr                 = NULL;
  boolean                   is_gw                         = TRUE;
  mmgsdi_aid_type           aid;
  boolean                   is_default_channel_in_use     = FALSE;
  byte                      channel_id                    = MMGSDI_DEFAULT_CHANNEL;
  uint8                     reserved_channels             = 0;
  uint8                     channel_index                 = MMGSDI_MAX_CHANNEL_INFO;

  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  memset(&aid, 0x00, sizeof(mmgsdi_aid_type));

  if((req_ptr->app_id_data.data_len <= 0) ||
     (req_ptr->app_id_data.data_len > MMGSDI_MAX_AID_LEN))
  {
    /* Possibly GSM/RUIM application */
    UIM_MSG_ERR_1("Incorrect app id len = 0x%x",req_ptr->app_id_data.data_len);
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_get_session_app_info(req_ptr->request_header.session_id,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   &session_info_ptr,
                                                   NULL);

  if(mmgsdi_status != MMGSDI_SUCCESS ||
     session_info_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  if ((session_info_ptr->session_type == MMGSDI_GW_PROV_PRI_SESSION) ||
      (session_info_ptr->session_type == MMGSDI_GW_PROV_SEC_SESSION) ||
      (session_info_ptr->session_type == MMGSDI_GW_PROV_TER_SESSION))
  {
    is_gw = TRUE;
  }
  else if ((session_info_ptr->session_type == MMGSDI_1X_PROV_PRI_SESSION) ||
           (session_info_ptr->session_type == MMGSDI_1X_PROV_SEC_SESSION) ||
           (session_info_ptr->session_type == MMGSDI_1X_PROV_TER_SESSION))
  {
    is_gw = FALSE;
  }

    if (is_gw)
    {
      aid.app_type = MMGSDI_APP_USIM;
    }
    else
    {
      aid.app_type = MMGSDI_APP_CSIM;
    }
    /* Copy AID information from req_ptr - structure copy */
    aid.aid = req_ptr->app_id_data;

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(req_ptr->request_header.slot_id);

  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  if((session_info_ptr->channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
     (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index] != NULL))
  {
    UIM_MSG_ERR_0("session already has a channel open");
    return MMGSDI_ERROR;
  }

  /* Only app data is required in below function.*/
  if (mmgsdi_util_check_is_channel_available_for_reuse_uicc(
        req_ptr->request_header.slot_id, &aid.aid, &channel_id, &channel_index))
  {
    /* If the app being activated is already selected on a channel, reuse
       that channel. Since we could be here as a result of session switching
       (to an app that was earlier logically deactivated but not its termination
       was skipped... in that case, make sure to reset the state of globals
       to indicate that the app is no longer logically terminated.*/
    if((mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_SKIP_PROV_APP_TERMINATION, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) ==
          MMGSDI_FEATURE_ENABLED) &&
       (channel_index < MMGSDI_MAX_CHANNEL_INFO) &&
       (mmgsdi_channel_info_ptr_table[channel_index] != NULL) &&
       (mmgsdi_channel_info_ptr_table[channel_index]->app_termination_skipped))
    {
      UIM_MSG_HIGH_2("Activating app whose termination is skipped during deactivation.Channel 0x%x Slot 0x%x",
                     channel_index,
                     req_ptr->request_header.slot_id);

      MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

      mmgsdi_channel_info_ptr_table[channel_index]->app_termination_skipped = FALSE;
      mmgsdi_channel_info_ptr_table[channel_index]->selective_act_after_app_term_skipped = TRUE;

      MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;
    }

    UIM_MSG_HIGH_1("Channel 0x%x available for reuse", channel_id);
    return mmgsdi_session_build_uim_open_channel_rsp(
            channel_id,
            req_ptr->request_header,
            &aid,
            UIM_PASS,
            FALSE);
  }
  else
  {
    /* Verify if there is a UICC app on the default channel. If not,
       default channel can be used */
    is_default_channel_in_use =
      mmgsdi_util_check_is_default_channel_in_use_uicc_apps(
        req_ptr->request_header.slot_id);
    if (!is_default_channel_in_use)
    {
      reserved_channels = 1;
    }

    /* In the case that an USIM application is present on the card,
       the default channel should be reserved for the USIM so that
       for the case where refresh is triggered without an AID, the
       USIM will be chosen as the default application to be refreshed  */
    if (aid.app_type != MMGSDI_APP_USIM &&
        is_default_channel_in_use == FALSE)
    {
      if (mmgsdi_util_get_usim_apps(req_ptr->request_header.slot_id) > 0)
      {
        is_default_channel_in_use = TRUE;
      }
    }

    if (is_default_channel_in_use)
    {
      if(slot_data_ptr->available_logical_channels > reserved_channels)
      {
        /* The app is not active on any channel and default channel is being
           used by another app. Send a manage channel command */
        req_ptr->request_header.request_type = MMGSDI_SESSION_OPEN_REQ;
        return  mmgsdi_uim_uicc_open_channel(
                    (const mmgsdi_request_header_type*)&req_ptr->request_header,
                    &aid, FALSE, FALSE, MMGSDI_SELECT_RSP_FCP);
      }
      else
      {
        return mmgsdi_session_build_uim_open_channel_rsp(
                MMGSDI_DEFAULT_CHANNEL, /* channel id */
                req_ptr->request_header,
                NULL,
                UIM_FAIL,
                FALSE);
      }
    }
    else
    {
      return mmgsdi_session_build_uim_open_channel_rsp(
              MMGSDI_DEFAULT_CHANNEL, /* channel id */
              req_ptr->request_header,
              &aid,
              UIM_PASS,
              FALSE);
    }
  } /* else (mmgsdi_util_check_is_channel_available_for_reuse_uicc())*/
}/*mmgsdi_session_uicc_open_channel_chk*/


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_RSP_ISIM_AUTH

   DESCRIPTION:
     This function handles the response for ISIM Authenication from the card

   DEPENDENCIES:


   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                              but the service table indicates the card does not
                              have the support

   SIDE EFFECTS:

-----------------------------------------------------------------------------*/
static mmgsdi_return_enum_type mmgsdi_session_rsp_isim_auth(
  const mmgsdi_uim_report_rsp_type *uim_rsp_ptr)
{
  mmgsdi_request_header_type            req_header;
  mmgsdi_return_enum_type               mmgsdi_status   = MMGSDI_ERROR;
  mmgsdi_return_enum_type               report_status   = MMGSDI_SUCCESS;
  int32                                 index           = 0;
  uim_cmd_type                        * uim_cmd_ptr     = NULL;
  mmgsdi_client_req_extra_info_type   * extra_param_ptr = NULL;
  int32                                 offset          = 0;
  int32                                 i               = 0;
  int32                                 j               = 0;
  mmgsdi_data_type                      temp_auth_rsp_buf[3];
  uint32                                auth_data_len   = 0;
  mmgsdi_isim_auth_data_type            auth_data;

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  memset(&auth_data, 0x00, sizeof(mmgsdi_isim_auth_data_type));
  memset(&temp_auth_rsp_buf[0], 0x00, sizeof(mmgsdi_data_type));
  memset(&temp_auth_rsp_buf[1], 0x00, sizeof(mmgsdi_data_type));
  memset(&temp_auth_rsp_buf[2], 0x00, sizeof(mmgsdi_data_type));

  /*---------------------------------------------------------------------------
    Obtain index
    Index should be [0, MMGSDI_MAX_CLIENT_INFO-1],
    Index is set when
    a) a success queuing of command to UIM
    b) a failure in queuing command to UIM but client_request pointer should
      be set in a valid index location in mmgsdi process command
  ---------------------------------------------------------------------------*/
  index = uim_rsp_ptr->rsp_data_index;

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);

  mmgsdi_status = mmgsdi_util_get_client_request_table_info(
                                index,
                                &req_header,
                                &uim_cmd_ptr,
                                &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_DEBUG_MSG_ERROR_0("Error in getting info fr client req table info");
    return mmgsdi_status;
  }

  if (!uim_rsp_ptr->is_uim_success)
  {
    /* mmgsdi status will be populated as a result of sw1, and sw2
       This will be passed into the build confirmation function */
    report_status = mmgsdi_uicc_rsp_parse_uim_response(
      uim_rsp_ptr);
  }

  /* parse authentication response into temporary data buffers */
  offset = 1;
  if (report_status == MMGSDI_SUCCESS &&
      uim_rsp_ptr->rsp_data.data_len > offset)
  {
    if ((mmgsdi_isim_auth_rsp_tag_enum_type)uim_rsp_ptr->rsp_data.data_ptr[0]
          == MMGSDI_ISIM_3G_AUTH_SUCCESS)
    {
      if (uim_cmd_ptr->isim_autn.ref_data_number == UIM_AUTN_ISIM_AKA ||
          uim_cmd_ptr->isim_autn.ref_data_number == UIM_AUTN_ISIM_HTTP_DIGEST)
      {
        uint8 max_resp_elements = 0;

        if (uim_cmd_ptr->isim_autn.ref_data_number == UIM_AUTN_ISIM_AKA)
        {
          /* For ISIM AKA, response has 3 elements: res, ck, and ik*/
          max_resp_elements = 3;
        }
        else
        {
          /* For ISIM HTTP Digest, response has 2 elements: res and key*/
          max_resp_elements = 2;
        }

        for (i = 0; i < max_resp_elements; i++)
        {
          if (uim_rsp_ptr->rsp_data.data_len <= offset)
          {
            for (j = 0; j < i; j++)
            {
              MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(temp_auth_rsp_buf[j].data_ptr);
              temp_auth_rsp_buf[j].data_len = 0;
            }
            report_status = MMGSDI_ERROR;
            break;
          }
          temp_auth_rsp_buf[i].data_len = uim_rsp_ptr->rsp_data.data_ptr[offset];
          auth_data_len = int32touint32(temp_auth_rsp_buf[i].data_len);

          if(auth_data_len > 0 &&
             auth_data_len < (uim_rsp_ptr->rsp_data.data_len - offset))
          {
            MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(temp_auth_rsp_buf[i].data_ptr,
                                               auth_data_len);
          }

          if (temp_auth_rsp_buf[i].data_ptr == NULL)
          {
            for (j = 0; j < i; j++)
            {
              MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(temp_auth_rsp_buf[j].data_ptr);
            }
            report_status = MMGSDI_ERROR;
            break;
          }
          offset++;
          mmgsdi_memscpy((void*)temp_auth_rsp_buf[i].data_ptr,
                         auth_data_len,
                         (void*)&uim_rsp_ptr->rsp_data.data_ptr[offset],
                         auth_data_len);
          offset += temp_auth_rsp_buf[i].data_len;
        }
      }/* if ref_data_number == UIM_AUTN_ISIM_AKA || UIM_AUTN_ISIM_HTTP_DIGEST */
      else if (uim_cmd_ptr->isim_autn.ref_data_number == UIM_AUTN_ISIM_GBA_DATA)
      {
        /* GBA response for GBA bootstrapping or GBA NAF derivation.
           Either way just store length/value in rsp_buf */
        temp_auth_rsp_buf[0].data_len = uim_rsp_ptr->rsp_data.data_ptr[offset];
        auth_data_len = int32touint32(temp_auth_rsp_buf[0].data_len);

        if(auth_data_len > 0 &&
           auth_data_len < (uim_rsp_ptr->rsp_data.data_len - offset))
        {
          MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(temp_auth_rsp_buf[0].data_ptr,
                                             auth_data_len);
        }

        if (temp_auth_rsp_buf[0].data_ptr != NULL)
        {
          offset++;
          mmgsdi_memscpy((void*)temp_auth_rsp_buf[0].data_ptr,
                         auth_data_len,
                         (void*)&uim_rsp_ptr->rsp_data.data_ptr[offset],
                         auth_data_len);
        }
        else
        {
          report_status = MMGSDI_ERROR;
        }
      }/* if ref_data_number == UIM_AUTN_ISIM_GBA_DATA */
    }/* if data_ptr == MMGSDI_ISIM_3G_AUTH_SUCCESS*/
    else if ((mmgsdi_isim_auth_rsp_tag_enum_type)uim_rsp_ptr->rsp_data.data_ptr[0]
               == MMGSDI_ISIM_SYNC_FAIL)
    {
      /* Authenication AUTS failure */
      temp_auth_rsp_buf[0].data_len = uim_rsp_ptr->rsp_data.data_ptr[offset];
      auth_data_len = int32touint32(temp_auth_rsp_buf[0].data_len);

      if(auth_data_len > 0 &&
         auth_data_len < (uim_rsp_ptr->rsp_data.data_len - offset))
      {
        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(temp_auth_rsp_buf[0].data_ptr,
                                           auth_data_len);
      }

      if (temp_auth_rsp_buf[0].data_ptr != NULL)
      {
        offset++;
        mmgsdi_memscpy((void*)temp_auth_rsp_buf[0].data_ptr,
                       auth_data_len,
                       (void*)&uim_rsp_ptr->rsp_data.data_ptr[offset],
                       auth_data_len);
        report_status = MMGSDI_AUTS_FAIL;
      }
      else
      {
        report_status = MMGSDI_ERROR;
      }
    }/* if data_ptr == MMGSDI_ISIM_SYNC_FAIL */
  }/* if offset > 1 */
  else
  {
    if (report_status == MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_0("Have data response, but not Succses or AUTS failure, ignore the data");
      report_status = MMGSDI_ERROR;
    }
  }

  auth_data.res = temp_auth_rsp_buf[0];
  auth_data.ck  = temp_auth_rsp_buf[1];
  auth_data.ik  = temp_auth_rsp_buf[2];

  mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status,
                                             &req_header,
                                             NULL,
                                             &auth_data,
                                             uim_rsp_ptr->cmd_transacted,
                                             uim_rsp_ptr->status_word);

  /* Free temp auth response data */
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(temp_auth_rsp_buf[0].data_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(temp_auth_rsp_buf[1].data_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(temp_auth_rsp_buf[2].data_ptr);

  return mmgsdi_status;
} /* mmgsdi_session_rsp_isim_auth */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_BUILD_SELECT_AID_FROM_INDICES

   DESCRIPTION:
     This function will build the select AID command and put it into the
     mmgsdi command queue

   DEPENDENCIES:


   LIMITATIONS:
     SESSION_SELECT_AID_REQ should be used for non default app only,
     for default app, use SELECT_AID_REQ

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                              but the service table indicates the card does not
                              have the support

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
static mmgsdi_return_enum_type mmgsdi_session_build_select_aid_from_indices(
  const mmgsdi_request_header_type *req_header_ptr,
  mmgsdi_static_data_type           aid_info,
  boolean                           select_rsp_requested,
  mmgsdi_select_rsp_enum_type       select_rsp_type)
{
  mmgsdi_session_info_type  * session_info_ptr  = NULL;
  mmgsdi_return_enum_type     mmgsdi_status     = MMGSDI_ERROR;

  MMGSDIUTIL_RETURN_IF_NULL(req_header_ptr);

  mmgsdi_status = mmgsdi_util_get_session_app_info(req_header_ptr->session_id,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   &session_info_ptr,
                                                   NULL);

  if(mmgsdi_status != MMGSDI_SUCCESS ||
     session_info_ptr == NULL ||
     (session_info_ptr->channel_info_index >= MMGSDI_MAX_CHANNEL_INFO) ||
     (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index] == NULL))
  {
    UIM_MSG_ERR_1("session/chnl ptr is NULL sesion_p=0x%x", session_info_ptr);
    return MMGSDI_ERROR;
  }

  /* orig_request_type should be OPEN or ACT_OR_SWITCH_PROV */
  return mmgsdi_session_build_select_aid(req_header_ptr->client_id,
                                         req_header_ptr->session_id,
                                         req_header_ptr->slot_id,
                                         req_header_ptr->orig_request_type,
                                         MMGSDI_SESSION_SELECT_AID_REQ,
                                         aid_info,
                                         select_rsp_requested,
                                         select_rsp_type,
                                         req_header_ptr->response_cb,
                                         req_header_ptr->client_data);
} /* mmgsdi_session_build_select_aid_from_indices */


/*===========================================================================
FUNCTION:      MMGSDI_SESSION_BUILD_SELECT_AID

DESCRIPTION:
  This function will build the select AID command and put it into the
  mmgsdi command queue

DEPENDENCIES:


LIMITATIONS:


RETURN VALUE:
  mmgsdi_return_enum_type

  MMGSDI_SUCCESS:          The command processing was successful.
  MMGSDI_ERROR:            The command processing was not successful.
  MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                           within appropriate ranges.
  MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                           but the service table indicates the card does not
                           have the support

SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_session_build_select_aid(
  mmgsdi_client_id_type              client_id,
  mmgsdi_session_id_type             session_id,
  mmgsdi_slot_id_enum_type           slot_id,
  mmgsdi_cmd_enum_type               orig_req_type,
  mmgsdi_cmd_enum_type               curr_req_type,
  mmgsdi_static_data_type            aid,
  boolean                            select_rsp_requested,
  mmgsdi_select_rsp_enum_type        select_rsp_type,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref)
{
  mmgsdi_task_cmd_type                 *task_cmd_ptr       = NULL;
  mmgsdi_access_type                    file_name;
  mmgsdi_return_enum_type               mmgsdi_status      = MMGSDI_SUCCESS;
  mmgsdi_get_file_attr_req_type        *get_file_attr_msg  = NULL;
  int32                                 total_mem_len      = 0;
  int32                                 var_len            = 0;
  int32                                 temp_mem           = 0;
  uint32                                task_cmd_len       = 0;
  mmgsdi_task_enum_type                 mmgsdi_task        = MMGSDI_TASK_MAX;

  UIM_MSG_HIGH_1("MMGSDI_SESSION_BUILD_SELECT_AID, session_id 0x%x", session_id);

  if ((curr_req_type != MMGSDI_SESSION_DEACTIVATE_REQ) &&
      (curr_req_type != MMGSDI_SESSION_SELECT_AID_REQ))
  {
    UIM_MSG_ERR_1("Curr Req Type should be either Deactivate or Select AID 0x%x",
                  curr_req_type);
    return MMGSDI_ERROR;
  }

  file_name.access_method = MMGSDI_BY_APP_ID_ACCESS;
  file_name.file.app_id.data_len = aid.data_len;

  mmgsdi_memscpy((void*)file_name.file.app_id.data_ptr,
                 sizeof(file_name.file.app_id.data_ptr),
                 (void*)aid.data_ptr,
                 int32touint32(file_name.file.app_id.data_len));

  var_len = file_name.file.app_id.data_len;

  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  mmgsdi_util_cal_align_size(var_len,
                              &temp_mem);

  MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(var_len,temp_mem);

  total_mem_len += temp_mem;

  task_cmd_len = int32touint32(total_mem_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /*---------------------------------------------------------------------------
    - Align pointers of the various structures members
  ---------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.get_file_attr_req,
                         curr_req_type);

  get_file_attr_msg = &task_cmd_ptr->cmd.cmd.get_file_attr_req;

  /*---------------------------------------------------------------------------
    POPULATE THE REQUEST MESSAGE PAYLOAD
    1. client_id       - Client ID Obtained
    2. session_id      - Session ID
    3. request_type    - Request Command Type
    4. request_len     - Length of message + header
    5. payload_len     - Length of the message payload
    6. slot_id         - Slot that the request is to be performed on
    7. client_data     - Pointer to Client Data
    8. response_cb     - Response Callback Pointer
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                          = curr_req_type;
  get_file_attr_msg->request_header.client_id         = client_id;
  get_file_attr_msg->request_header.session_id        = session_id;
  get_file_attr_msg->request_header.request_type      = curr_req_type;
  get_file_attr_msg->request_header.orig_request_type = orig_req_type;
  get_file_attr_msg->request_header.slot_id           = slot_id;
  get_file_attr_msg->request_header.client_data       = client_ref;
  get_file_attr_msg->request_header.response_cb       = response_cb_ptr;
  get_file_attr_msg->request_header.request_len       = total_mem_len;
  get_file_attr_msg->request_header.payload_len       = get_file_attr_msg->request_header.request_len -
                                                          uint32toint32(sizeof(mmgsdi_request_header_type));

  /*---------------------------------------------------------------------------
    POPULATE THE REQUEST MESSAGE PAYLOAD
    1.  access_method       -  Access type specified in the function paramters
    2.  file_enum           -  File name specified in the function paramters
                           OR
    2a. path                -  Path specified in the function paramters
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_copy_access_type(&get_file_attr_msg->access,
    &file_name);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_DEBUG_MSG_ERROR_1("Get File Attr Lib Fail: Access Copy Fail 0x%x",
                                 mmgsdi_status);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(task_cmd_ptr);
    return mmgsdi_status;
  }

  if (curr_req_type == MMGSDI_SESSION_DEACTIVATE_REQ)
  {
    get_file_attr_msg->activate_aid = FALSE;

    /* Set in_priority_queue flag to TRUE, to queue req in high_priority_cmd_q
       because session_deactivate req is result of session_close req and should
       be handled before handling next session_activation req from task_cmd_q*/
    task_cmd_ptr->cmd.cmd.session_deact_req.in_priority_queue = TRUE;
  }
  else if (curr_req_type == MMGSDI_SESSION_SELECT_AID_REQ)
  {
    get_file_attr_msg->activate_aid = TRUE;
  }

  if (orig_req_type == MMGSDI_SESSION_OPEN_REQ ||
      orig_req_type == MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_REQ ||
      orig_req_type == MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ)
  {
    get_file_attr_msg->select_rsp_requested = select_rsp_requested;
    get_file_attr_msg->select_rsp_type      = select_rsp_type;
  }

  /* Queue SELECT AID REQ to SLOT SPECIFIC TASK instead of MAIN TASK to get it
     processed before any other request. It is possible that SESSION OPEN REQ
     is already queued to SLOT SPECIFIC TASK to open a channel and select same
     application. If SELECT AID REQ is queued to MAIN TASK and SLOT SPECIFIC
     TASK gets signal to process the command, it will open one more channel to
     select same application; which is wrong. If SELECT AID REQ is queued to
     SLOT SPECIFIC TASK, it will be processed first (as it is in high priority
     command queue). */
  mmgsdi_task   = mmgsdi_util_get_task_from_slot(slot_id);
  mmgsdi_status = mmgsdi_cmd_ext(task_cmd_ptr, mmgsdi_task);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(task_cmd_ptr);

  }
  return mmgsdi_status;
} /* mmgsdi_session_build_select_aid */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_BUILD_CLOSE_REQ

   DESCRIPTION:
     This function build the close channel management request and put it onto
     the MMGSDI task queue.

   DEPENDENCIES:


   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                              but the service table indicates the card does not
                              have the support

   SIDE EFFECTS:

-----------------------------------------------------------------------------*/
static mmgsdi_return_enum_type mmgsdi_session_build_close_req (
  const mmgsdi_request_header_type           * req_header_ptr
)
{
  mmgsdi_task_cmd_type                 *task_cmd_ptr  = NULL;
  mmgsdi_session_close_req_type        *close_msg      = NULL;
  mmgsdi_return_enum_type               mmgsdi_status  = MMGSDI_SUCCESS;
  int32                                 total_mem_len  = 0;
  uint32                                task_cmd_len   = 0;

  MMGSDIUTIL_RETURN_IF_NULL(req_header_ptr);

  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  task_cmd_len = int32touint32(total_mem_len);
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - No need to align pointers of the various structures members
  ---------------------------------------------------------------------------*/
  close_msg = &task_cmd_ptr->cmd.cmd.session_close_req;

  /*---------------------------------------------------------------------------
    POPULATE THE REQUEST MESSAGE PAYLOAD
    1. client_id       - Client ID Obtained
    2. request_type    - Request Command Type
    3. request_len     - Length of message + header
    4. payload_len     - Length of the message payload
    5. slot_id         - Slot that the request is to be performed on
    6. client_data     - Pointer to Client Data
    7. response_cb     - Response Callback Pointer
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                  = MMGSDI_SESSION_CLOSE_REQ;
  close_msg->request_header.client_id         = req_header_ptr->client_id;
  close_msg->request_header.session_id        = req_header_ptr->session_id;
  close_msg->request_header.request_type      = MMGSDI_SESSION_CLOSE_REQ;
  close_msg->request_header.orig_request_type = req_header_ptr->orig_request_type;
  close_msg->request_header.slot_id           = req_header_ptr->slot_id;
  close_msg->request_header.client_data       = req_header_ptr->client_data;
  close_msg->request_header.response_cb       = req_header_ptr->response_cb;
  close_msg->request_header.request_len       = total_mem_len;
  close_msg->request_header.payload_len       = total_mem_len -
                                                uint32toint32(sizeof(mmgsdi_request_header_type));

  /* PLACE ON GSDI QUEUE */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(task_cmd_ptr);

  }
  return mmgsdi_status;
}

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_OPEN_FAILED_CLEANUP

   DESCRIPTION:
     This function based on whether a channel has been opened successfully
     or not, it will build a request to close the channel.  In additionl,
     It will perform clean up on the session info table.

     A channel is opened, but either during open rsp processing or
     select aid processing it failed, therefore, the complete open procedure
     failed. In that case we need to close the channel and send back a
     failure.  The confirmation and cleanup process are being handled
     in close response processing.

     If a channel failed to open, then we are only required to cleanup entried
     in client id reg table and build failure confirmation to client

   DEPENDENCIES:


   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:
     client id in the req_header_ptr will be updated to remove the session ID

-----------------------------------------------------------------------------*/
static mmgsdi_return_enum_type mmgsdi_session_open_failed_cleanup (
  mmgsdi_request_header_type       *req_header_ptr,
  boolean                           channel_opened,
  mmgsdi_return_enum_type           report_status
)
{
  mmgsdi_return_enum_type   mmgsdi_status      = MMGSDI_SUCCESS;
  mmgsdi_session_info_type  *session_info_ptr  = NULL;

  MMGSDIUTIL_RETURN_IF_NULL(req_header_ptr);

  if (channel_opened)
  {
    mmgsdi_status = mmgsdi_session_build_close_req(req_header_ptr);

    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_1("Unable to build the close channel request, status: 0x%x",
                    mmgsdi_status);
    }
    return mmgsdi_status;
  }

  /* channel was not opened, just need to perform client id reg table cleanup */

  mmgsdi_status = mmgsdi_util_get_session_app_info(req_header_ptr->session_id,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   &session_info_ptr,
                                                   NULL);

  if(mmgsdi_status != MMGSDI_SUCCESS ||
     session_info_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* Cleanup session information data. */
  mmgsdi_status = mmgsdi_session_release_info(session_info_ptr,
                                              req_header_ptr->orig_request_type);

  req_header_ptr->session_id = MMGSDI_INVALID_SESSION_ID;

  /* confirmation to client will be sent by mmgsdi_build_and_queue_static_cnf */
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_HIGH_2("SESSION_RSP - OPEN FAILED cleanup status 0x%x, mmgsdi_status 0x%x",
                   report_status,
                   mmgsdi_status);
  }
  return mmgsdi_status;
}/*mmgsdi_session_open_failed_cleanup*/


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_RSP_SELECT_AID

   DESCRIPTION:
     This function handles the response for Select on AID returned by the
     card.  It also builds a confirmation message to the client. Select
     AID request is queued as a part of ACTIVATE_OR_SWITCH prov app request

   DEPENDENCIES:


   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                              but the service table indicates the card does not
                              have the support

   SIDE EFFECTS:
    None.

   CRITICAL SECTIONS:
     The function uses mmgsdi_client_app_data_crit_sect_ptr to protect accesses
     to session info ptr and channel info ptr.
-------------------------------------------------------------------------------*/
static mmgsdi_return_enum_type mmgsdi_session_rsp_select_aid(
  const mmgsdi_uim_report_rsp_type *uim_rsp_ptr)
{
  mmgsdi_request_header_type            req_header          = {0};
  mmgsdi_return_enum_type               mmgsdi_status       = MMGSDI_SUCCESS;
  mmgsdi_return_enum_type               report_status       = MMGSDI_ERROR;
  mmgsdi_return_enum_type               cnf_status          = MMGSDI_ERROR;
  int32                                 index               = 0;
  uim_cmd_type                        * uim_cmd_ptr         = NULL;
  mmgsdi_client_req_extra_info_type   * extra_param_ptr     = NULL;
  mmgsdi_session_info_type            * session_info_ptr    = NULL;
  boolean                               aid_added_to_table  = FALSE;
  mmgsdi_sw_status_type                 sw_status           = {0};
  boolean                               channel_opened      = FALSE;

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);
  /*---------------------------------------------------------------------------
    Obtain index
    Index should be [0, MMGSDI_MAX_CLIENT_INFO-1],
    Index is set when
    a) a success queuing of command to UIM
    b) a failure in queuing command to UIM but client_request pointer should
      be set in a valid index location in mmgsdi process command
  ---------------------------------------------------------------------------*/
  index = uim_rsp_ptr->rsp_data_index;

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);

  mmgsdi_status = mmgsdi_util_get_client_request_table_info(
                                index,
                                &req_header,
                                &uim_cmd_ptr,
                                &extra_param_ptr);

  /* Extra param could be NULL if the SELECT operation failed synchronously
     due to lack of support of the card for the operation */
  if (mmgsdi_status == MMGSDI_SUCCESS &&
      extra_param_ptr == NULL &&
      uim_rsp_ptr->mmgsdi_error_status != MMGSDI_SUCCESS)
  {
    return uim_rsp_ptr->mmgsdi_error_status;
  }

  if (mmgsdi_status != MMGSDI_SUCCESS ||
      extra_param_ptr == NULL)
  {
    UIM_MSG_ERR_1("Invalid client req table info, extra_param=0x%x",
              extra_param_ptr);
    /* no way to perform open cleanup since client req table entry is invalid */
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_get_session_app_info(req_header.session_id,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   &session_info_ptr,
                                                   NULL);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* no way to perform open cleanup since client table entry is invalid */
    return MMGSDI_ERROR;
  }

  /* Copy status words to local variable so that they can be updated later. */
  sw_status.sw1 = uim_rsp_ptr->status_word.sw1;
  sw_status.sw2 = uim_rsp_ptr->status_word.sw2;

  /* If the intermediate SW contains 62 XX, overwrite final SW with the intermediate SW */
  if(mmgsdi_util_is_non_prov_session(session_info_ptr->session_type) &&
     ((uim_rsp_ptr->get_resp_status_word.sw1 == GSDI_SW1_WARNINGS_PART_1) &&
     (uim_rsp_ptr->get_resp_status_word.sw2 == GSDI_SW2_NO_INFO_VOLATILE_MEM_UNCHANGED ||
      uim_rsp_ptr->get_resp_status_word.sw2 == GSDI_SW2_PART_OF_DATA_MAY_BE_CORRUPTED ||
      uim_rsp_ptr->get_resp_status_word.sw2 == GSDI_SW2_END_REACHED_BEFORE_LE_BYTES ||
      uim_rsp_ptr->get_resp_status_word.sw2 == GSDI_SW2_SELECTED_FILE_INVALIDATED)))
  {
    UIM_MSG_HIGH_0("Received intermediate status word as 62 xx ");
    sw_status.sw1 = uim_rsp_ptr->get_resp_status_word.sw1;
    sw_status.sw2 = uim_rsp_ptr->get_resp_status_word.sw2;
  }

  if (!uim_rsp_ptr->is_uim_success)
  {
    /* mmgsdi status will be populated as a result of sw1, and sw2
       This will be passed into the build confirmation function.
       Ignore certain warnings returned by card while opening a non-prov
       session. */
    if(mmgsdi_util_is_non_prov_session(session_info_ptr->session_type) &&
       (sw_status.sw1 == GSDI_SW1_WARNINGS_PART_1) &&
       (sw_status.sw2 == GSDI_SW2_NO_INFO_VOLATILE_MEM_UNCHANGED ||
        sw_status.sw2 == GSDI_SW2_PART_OF_DATA_MAY_BE_CORRUPTED ||
        sw_status.sw2 == GSDI_SW2_END_REACHED_BEFORE_LE_BYTES ||
        sw_status.sw2 == GSDI_SW2_SELECTED_FILE_INVALIDATED))
    {
      UIM_MSG_HIGH_0("Ignore warning in SELECT AID response");
      report_status = MMGSDI_SUCCESS;
    }
    else
    {
      report_status = mmgsdi_uicc_rsp_parse_uim_response(uim_rsp_ptr);
    }
  }
  else
  {
    report_status = MMGSDI_SUCCESS;
  }

  if (req_header.orig_request_type == MMGSDI_SESSION_OPEN_REQ ||
      req_header.orig_request_type == MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_REQ ||
      req_header.orig_request_type == MMGSDI_SESSION_SELECT_AID_REQ)
  {
    mmgsdi_session_cnf_add_info_type      select_aid_cnf_info;
    memset(&select_aid_cnf_info, 0x00, sizeof(mmgsdi_session_cnf_add_info_type));

    /* If the selected application was present in the APP INFO table
       prior to selection then we can copy the entire APP data including
       the AID, type and label from the APP INFO ptr. If not we need
       to peek into the extra param info and get the AID and type */
    if((session_info_ptr->channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
       (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index] != NULL) &&
       (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr != NULL))
    {
      mmgsdi_memscpy((void*)&select_aid_cnf_info.app_info,
                     sizeof(select_aid_cnf_info.app_info),
                     (void*)&(mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->app_data),
                     sizeof(mmgsdi_aid_type));
    }
    else
    {
      mmgsdi_memscpy((void*)&select_aid_cnf_info.app_info.aid,
        sizeof(select_aid_cnf_info.app_info.aid),
        (void*)&extra_param_ptr->get_file_attr_data.access.file.app_id,
        sizeof(mmgsdi_static_data_type));

      select_aid_cnf_info.app_info.app_type =
        mmgsdi_util_determine_app_type_from_aid(&select_aid_cnf_info.app_info.aid);
    }

    select_aid_cnf_info.select_rsp_requested =
      extra_param_ptr->get_file_attr_data.select_rsp_requested;
    select_aid_cnf_info.select_rsp_type =
      extra_param_ptr->get_file_attr_data.select_rsp_type;

    if (select_aid_cnf_info.select_rsp_requested)
    {
      if ((uim_rsp_ptr->rsp_data.data_ptr != NULL) &&
          (uim_rsp_ptr->rsp_data.data_len > 0))
      {
        select_aid_cnf_info.rsp_data.data_ptr = uim_rsp_ptr->rsp_data.data_ptr;
        select_aid_cnf_info.rsp_data.data_len = uim_rsp_ptr->rsp_data.data_len;
      }
    }

    /* SELECT AID response for non-prov application */
    if (report_status == MMGSDI_SUCCESS)
    {
      /* Add the AID if it is not present in the mmgsdi session app info table */
      mmgsdi_status = mmgsdi_session_add_aid_if_not_present(&select_aid_cnf_info.app_info,
                                                            session_info_ptr,
                                                            &aid_added_to_table);

      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        /* Unable to add AID to app table, send back a failure conf
           followed up session clean up and close channel */
        UIM_MSG_ERR_1("After Manage Channel,AID addition to APP PIN table failed",
                      mmgsdi_status);
        sw_status.sw1 = 0xFF;
        sw_status.sw2 = 0xFF;
        cnf_status = mmgsdi_cnf_build_and_queue(MMGSDI_SELECT_AID_FAILED,
                                                &req_header,
                                                NULL,
                                                &select_aid_cnf_info,
                                                FALSE,
                                                sw_status);

        /* Since client has been notified about the select AID failure
           call a dummy callback to wrap up the close channel request */
        req_header.response_cb = mmgsdi_session_close_channel_internal_cb;

        mmgsdi_status =  mmgsdi_session_open_failed_cleanup(&req_header,
                                                            TRUE,
                                                            report_status);

        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          UIM_MSG_ERR_2("Error in session cleanup, session_id=0x%x,status=0x%x",
                        session_info_ptr->session_id, mmgsdi_status);
        }
        return cnf_status;
      } /* (mmgsdi_status != MMGSDI_SUCCESS)) */
      else if((mmgsdi_status == MMGSDI_SUCCESS) && aid_added_to_table)
      {
        /* Activate the newly selected application if added.
           mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr
           will point to new app */
        mmgsdi_status = mmgsdi_session_activate_app_info(req_header.session_id,
                                                         select_aid_cnf_info.app_info.aid);
        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          UIM_MSG_ERR_0("Activate App Info failed");
          return MMGSDI_ERROR;
        }
      } /* else if((mmgsdi_status == MMGSDI_SUCCESS) && aid_added_to_table) */

      /* Populate table */
      mmgsdi_status = mmgsdi_session_populate_session_info(
        uim_rsp_ptr,
        &req_header,
        (const mmgsdi_get_file_info_extra_info_type*)&extra_param_ptr->get_file_attr_data);

      if (mmgsdi_status == MMGSDI_SUCCESS)
      {
        if((session_info_ptr->channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
           (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index] != NULL))
        {
          if(mmgsdi_uicc_rsp_parse_select_response_adf(
               mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr,
               uim_rsp_ptr->rsp_data.data_ptr,
               (uint16)uim_rsp_ptr->rsp_data.data_len,
               extra_param_ptr->get_file_attr_data.select_rsp_type) !=
             MMGSDI_SUCCESS)
          {
            UIM_MSG_ERR_0("SELECT response could not be parsed");
          }
          else
          {
            UIM_MSG_HIGH_2("Length of AID in SELECT request: 0x%x, in FCP: 0x%x",
                           mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->app_data.aid.data_len,
                           mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->aid_in_fcp.data_len);

            if((mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->aid_in_fcp.data_len > 0) &&
               (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->app_data.aid.data_len > 0))
            {
              if(((mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->aid_in_fcp.data_len >=
                   mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->app_data.aid.data_len) &&
                   (memcmp((void *)mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->aid_in_fcp.data_ptr,
                           (void *)mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->app_data.aid.data_ptr,
                           mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->app_data.aid.data_len)))
                           ||
                 (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->aid_in_fcp.data_len <
                  mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->app_data.aid.data_len))
              {
                /* Error and Return*/
                UIM_MSG_ERR_0("After Manage Channel, AID mismatch in SELECT request and response");
                sw_status.sw1 = 0xFF;
                sw_status.sw2 = 0xFF;
                cnf_status = mmgsdi_cnf_build_and_queue(MMGSDI_SELECT_AID_FAILED,
                                                        &req_header,
                                                        NULL,
                                                        &select_aid_cnf_info,
                                                        FALSE,
                                                        sw_status);

                /* Since client has been notified about the select AID failure
                   call a dummy callback to wrap up the close channel request */
                req_header.response_cb = mmgsdi_session_close_channel_internal_cb;
                mmgsdi_status =  mmgsdi_session_open_failed_cleanup(&req_header,
                                                                    TRUE,
                                                                    report_status);

                if(mmgsdi_status != MMGSDI_SUCCESS)
                {
                  UIM_MSG_ERR_2("Error in session cleanup, session_id=0x%x,status=0x%x",
                                session_info_ptr->session_id, mmgsdi_status);
                }
                return cnf_status;
              } /* memcmp()*/
              else
              {
                UIM_MSG_HIGH_0("After Manage Channel, AID matched in SELECT request and response");
              }
            }
          }
        }

        UIM_MSG_HIGH_0("SESSION_RSP - SELECT AID SUCCESS");
        return mmgsdi_cnf_build_and_queue(mmgsdi_status,
                                          &req_header,
                                          NULL,
                                          &select_aid_cnf_info,
                                          TRUE, /* sw required */
                                          sw_status);
      }
    } /* if (report_status == MMGSDI_SUCCESS) */
    else if (report_status == MMGSDI_NOT_FOUND &&
             req_header.orig_request_type != MMGSDI_SESSION_SELECT_AID_REQ)
    {
      /* SELECT AID failed becase AID not found. Lets add the AID to the global
         which can then be used at a later time to ensure a SELECT AID is never
         attempted again on this AID.
         Note that if its the RESELECTING of the AID that failed, we do not
         want to do the above. It is very possible and not an uncommon scenario
         that that first time SELECT on an AID is successful but a RESELECT
         (next/last/first/previous) fails... in that case, we do not want to
         block a SELECT AID command.
         Another thing to note is that orig_request_type is equal to
         MMGSDI_SESSION_SELECT_AID_REQ only if for the RESELCT API call
         using mmgsdi_session_reselect_aid. */
      mmgsdi_slot_data_type    * slot_data_ptr   = NULL;

      MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

      slot_data_ptr = mmgsdi_util_get_slot_data_ptr(req_header.slot_id);

      if (slot_data_ptr)
      {
        mmgsdi_aid_select_failed_node_type    *node_ptr = NULL;

        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(node_ptr,
                                           sizeof(mmgsdi_aid_select_failed_node_type));
        if (node_ptr != NULL)
        {
          mmgsdi_memscpy((void*)&node_ptr->app_info,
             sizeof(node_ptr->app_info),
             (void*)&select_aid_cnf_info.app_info,
             sizeof(select_aid_cnf_info.app_info));
          node_ptr->sw = sw_status;
          node_ptr->next_ptr = slot_data_ptr->aid_select_failed_ptr;
          slot_data_ptr->aid_select_failed_ptr = node_ptr;
        }
      }

      MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;
    }

    /* Card returned failure to select AID */
    UIM_MSG_HIGH_2("After Manage Channel,Select AID failed.sw1=0x%x,sw2=0x%x",
                   sw_status.sw1, sw_status.sw2);
    cnf_status = mmgsdi_cnf_build_and_queue(MMGSDI_SELECT_AID_FAILED,
                                            &req_header,
                                            NULL,
                                            &select_aid_cnf_info,
                                            TRUE,
                                            sw_status);

    if (req_header.orig_request_type != MMGSDI_SESSION_SELECT_AID_REQ)
    {
      /* In case of failure in selecting the next/last/first/previous app on a
         channel, the app that was selected earlier remains selected. Hence,
         the app info is not cleaned in case of reselect AID request. */

      /* set app info to the channel info to NULL because select aid failed */
      if((session_info_ptr->channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
         (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index] != NULL))
      {
        mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr = NULL;
      }

      /* Since client has been notified about the select AID failure
         call a dummy callback to wrap up the close channel request */
      req_header.response_cb = mmgsdi_session_close_channel_internal_cb;

      mmgsdi_status =  mmgsdi_session_open_failed_cleanup(&req_header,
                                                          TRUE,
                                                          report_status);

      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_1("Select AID rsp session cleanup failed,status=0x%x",
                      mmgsdi_status);
      }
    }
    return cnf_status;
  }
  else if (req_header.orig_request_type == MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ)
  {
    if (report_status == MMGSDI_SUCCESS)
    {
      /* Map sessions */
      mmgsdi_status = mmgsdi_session_populate_session_info(
        uim_rsp_ptr,
        &req_header,
        (const mmgsdi_get_file_info_extra_info_type*)&extra_param_ptr->get_file_attr_data);

       /* If Session info could not be populated, treat this as an error */
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_0("Could not populate Session info");
        report_status = MMGSDI_ERROR;
      }
    } /* if (report_status == MMGSDI_SUCCESS) */

    /* Channel info should be available at this point */
    if((session_info_ptr->channel_info_index >= MMGSDI_MAX_CHANNEL_INFO) ||
       (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index] == NULL))
    {
      UIM_MSG_ERR_1("No channel/app available for session_id=0x%x, chnl_ptr=0x%x",
                    session_info_ptr->session_id);
      return MMGSDI_ERROR;
    }

    if(report_status == MMGSDI_SUCCESS)
    {
      if(mmgsdi_uicc_rsp_parse_select_response_adf(
           mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr,
           uim_rsp_ptr->rsp_data.data_ptr,
           (uint16)uim_rsp_ptr->rsp_data.data_len,
           extra_param_ptr->get_file_attr_data.select_rsp_type) !=
         MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_0("SELECT response could not be parsed");
      }
    }

    UIM_MSG_HIGH_1("SESSION_RSP - ACTIVATE OR SWITCH PROV status 0x%x",
                   report_status);

    /* Queue the Activate/Switch confirmation */
    cnf_status =  mmgsdi_cnf_build_and_queue(report_status,
                                             &req_header,
                                             NULL,
                                             NULL,
                                             FALSE, /* sw not required for session
                                                       activate or switch prov */
                                             sw_status);

    /* Return if Application selection was successful */
    if(report_status == MMGSDI_SUCCESS)
    {
      return cnf_status;
    }

    if(mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_CHANGE_INST_CLASS, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) == MMGSDI_FEATURE_ENABLED)
    {
      /* Switch instruction class to ICC if SW1/SW2 of 0x69/0x99 or
         0x6F/0x00 (technical problem) is received as Select AID response */
      if((report_status == MMGSDI_WRONG_CLASS) ||
         ((report_status == MMGSDI_SIM_TECHNICAL_PROBLEMS) &&
          (sw_status.sw1 == GSDI_SW1_TECH_PROB_NO_DIAG_GIVEN)))
      {
        /* Clean-up the Session */
        mmgsdi_status =  mmgsdi_session_open_failed_cleanup(&req_header,
                                                            FALSE,
                                                            report_status);

        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          UIM_MSG_ERR_1("Select AID rsp session cleanup failed,status=0x%x",
                        mmgsdi_status);
        }

        /* Send out CARD_ERROR event to indicate that the UICC
           card is no longer available */
        mmgsdi_evt_build_and_send_card_error(TRUE,
                                             0,
                                             req_header.slot_id,
                                             MMGSDI_CARD_ERR_UNKNOWN_ERROR);

      /* Build Card Reset command */
      mmgsdi_status = mmgsdi_gen_build_card_reset(req_header.client_id,
                                                  req_header.slot_id,
                                                  MMGSDI_CARD_RESET_REQ,
                                                  UIM_ICC,
                                                  MMGSDI_RESET_CAUSE_GENERAL,
                                                  mmgsdi_util_internal_cb,
                                                  0);

        /* Reset Session and application data */
        if (mmgsdi_status == MMGSDI_SUCCESS)
        {
          UIM_MSG_HIGH_0("Reset Session and application data");
          mmgsdi_util_preinit_mmgsdi(0,
                                     MMGSDI_APP_NONE,
                                     MMGSDI_REFRESH_RESET,
                                     req_header.slot_id,
                                     FALSE);
        }
        return cnf_status;
      }
    } /* mmgsdi feature change_inst_class enabled */
    /* If a channel was opened for the application, this needs to be closed */
    if(mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->channel_id !=
       MMGSDI_DEFAULT_CHANNEL)
    {
      channel_opened = TRUE;
    }

    /* Since client has been notified about the prov. app. activation
       failure, call a dummy callback to wrap up the close channel request */
    req_header.response_cb = mmgsdi_session_close_channel_internal_cb;

    mmgsdi_status =  mmgsdi_session_open_failed_cleanup(&req_header,
                                                        channel_opened,
                                                        report_status);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_1("Select AID rsp session cleanup failed,status=0x%x",
                    mmgsdi_status);
    }
    return cnf_status;
  }
  else if (req_header.orig_request_type == MMGSDI_SESSION_APP_RESET_ACTIVATE_REQ)
  {
    MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;
    if (report_status == MMGSDI_SUCCESS)
    {
      /* Reset app and perso states */
      if((session_info_ptr->channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
         (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index] != NULL) &&
         (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr != NULL))
      {
        if(mmgsdi_util_is_prov_session(session_info_ptr->session_type))
        {
          mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->app_state =
            MMGSDI_APP_STATE_ACTIVATED;
        }
        else
        {
          mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->app_state =
            MMGSDI_APP_STATE_READY;
        }
        mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->app_selected =
          TRUE;
        mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->perso_state =
          MMGSDI_SIMLOCK_NOT_VERIFIED;

        if(mmgsdi_uicc_rsp_parse_select_response_adf(
             mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr,
             uim_rsp_ptr->rsp_data.data_ptr,
             (uint16)uim_rsp_ptr->rsp_data.data_len,
             extra_param_ptr->get_file_attr_data.select_rsp_type) !=
           MMGSDI_SUCCESS)
        {
          UIM_MSG_ERR_0("SELECT response could not be parsed");
        }
      }
      mmgsdi_status = mmgsdi_uicc_rsp_decode_pin_header_data (
                        &req_header,
                        (const mmgsdi_get_file_info_extra_info_type*)&extra_param_ptr->get_file_attr_data,
                        uim_rsp_ptr->rsp_data.data_ptr,
                        uim_rsp_ptr->rsp_data.data_len);

      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        report_status = mmgsdi_status;
      }
    }

    MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;
    return mmgsdi_cnf_build_and_queue(report_status,
                                      &req_header,
                                      NULL,
                                      NULL,
                                      FALSE,
                                      sw_status);
  }
  else
  {
    /* can't build a "valid" confirmation from the context of this function,
       return to the process_response so that the static_cnf can be used to
       build a generic confirmation */
    UIM_MSG_ERR_1("Unknown orig_req in select_aid_rsp 0x%x",
                  req_header.orig_request_type);
    return MMGSDI_ERROR;
  }
} /* mmgsdi_session_rsp_select_aid */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_SELECT_AID

   DESCRIPTION:
     This function builds command to select the AID

   DEPENDENCIES:


   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                              but the service table indicates the card does not
                              have the support

   SIDE EFFECTS:

-----------------------------------------------------------------------------*/
static mmgsdi_return_enum_type mmgsdi_session_select_aid (
  mmgsdi_get_file_attr_req_type * msg_ptr
)
{
  mmgsdi_get_file_attr_cnf_type   * cnf_ptr          = NULL;
  mmgsdi_session_info_type        * session_info_ptr = NULL;
  mmgsdi_return_enum_type           mmgsdi_status    = MMGSDI_ERROR;
  mmgsdi_slot_data_type           * slot_data_ptr    = NULL;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_p                                  - Cannot be NULL
    2. req_p->request_header.response_cb      - Cannot be NULL
    3. req_p->access.access_method            - MMGSDI_EF_ENUM_ACCESS
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL_2(msg_ptr,
                              msg_ptr->request_header.response_cb);

  switch (msg_ptr->access.access_method)
  {
    case MMGSDI_BY_APP_ID_ACCESS:
      break;

    default:
      UIM_MSG_ERR_1("Invalid Access Type 0x%x",
                    msg_ptr->access.access_method);
      return MMGSDI_INCORRECT_PARAMS;
  }

  UIM_MSG_HIGH_2("Sending down UICC SELECT AID command to MMGSDI UIM len 0x%x, access_method: 0x%x",
                 msg_ptr->access.file.app_id.data_len,
                 msg_ptr->access.access_method);

  /* Check if the client table's app info is valid or not, if null, populate
     with the requested data */
  if (msg_ptr->activate_aid)
  {
    if ( MMGSDI_SUCCESS != mmgsdi_session_activate_app_info(
                            msg_ptr->request_header.session_id,
                            msg_ptr->access.file.app_id))
    {
      UIM_MSG_ERR_0("Activate App Info failed");
    }

    /* If the original request is SELECT AID on a non-prov session, that means
       the channel is used for APDU streaming. Set the channel state to APDU
       streaming done so that it will not be recovered in case recovery is triggered.*/
    if (msg_ptr->request_header.orig_request_type == MMGSDI_SESSION_SELECT_AID_REQ)
    {

      mmgsdi_status = mmgsdi_util_get_session_app_info(msg_ptr->request_header.session_id,
                                                       NULL,
                                                       NULL,
                                                       NULL,
                                                       NULL,
                                                       &session_info_ptr,
                                                       NULL);

      if(mmgsdi_status != MMGSDI_SUCCESS ||
         session_info_ptr == NULL)
      {
        return MMGSDI_ERROR;
      }

      if (mmgsdi_util_is_non_prov_session(session_info_ptr->session_type) &&
          session_info_ptr->channel_info_index < MMGSDI_MAX_CHANNEL_INFO)
      {
        slot_data_ptr = mmgsdi_util_get_slot_data_ptr(
                          mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->slot_id);

        if (slot_data_ptr == NULL)
        {
          return MMGSDI_ERROR;
        }

        if (!slot_data_ptr->parsed_atr.select_by_part_df_name)
        {
          UIM_MSG_ERR_0("Card does not support selection by partial AID");
          return MMGSDI_NOT_SUPPORTED;
        }

        msg_ptr->select_rsp_requested = TRUE;
        msg_ptr->select_rsp_type =
          mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->select_rsp_type;

        if (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr == NULL)
        {
          UIM_MSG_ERR_1("No AID currently active on channel 0x%x",
                        mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->channel_id );
          return MMGSDI_ERROR;
        }

        mmgsdi_memscpy(&msg_ptr->access.file.app_id,
                       sizeof(mmgsdi_static_data_type),
                       &mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->app_data.aid,
                       sizeof(mmgsdi_static_data_type));

        if (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->channel_state ==
            MMGSDI_CHANNEL_STATE_READY)
        {
          MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;
          mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->channel_state =
            MMGSDI_CHANNEL_STATE_APDU_STREAMING_DONE;
          MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;
        }
      }
    }
  }
  return mmgsdi_uim_uicc_select ( msg_ptr,
                                  FALSE,
                                  &cnf_ptr,
                                  TRUE);
}/*mmgsdi_session_select_aid*/


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_RSP_CLOSE

   DESCRIPTION:
     This function handles the response for Close Channel Management.  It also
     builds the confirmation message and put it ont ot he MMGSDI task queue.

   DEPENDENCIES:


   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                              within appropriate ranges.

   SIDE EFFECTS:
     None.

   CRITICAL SECTIONS:
     The function uses mmgsdi_client_app_data_crit_sect_ptr to protect accesses
     to session info ptr and channel info ptr.
-----------------------------------------------------------------------------*/
static mmgsdi_return_enum_type mmgsdi_session_rsp_close(
  const mmgsdi_uim_report_rsp_type *uim_rsp_ptr)
{
  mmgsdi_request_header_type            req_header;
  mmgsdi_return_enum_type               mmgsdi_status        = MMGSDI_SUCCESS;
  int32                                 index                = 0;
  uim_cmd_type                        * uim_cmd_ptr          = NULL;
  mmgsdi_client_req_extra_info_type   * extra_param_ptr      = NULL;
  mmgsdi_session_info_type            * session_info_ptr     = NULL;
  mmgsdi_channel_info_type            * channel_info_ptr     = NULL;
  mmgsdi_session_cnf_add_info_type      session_cnf_info;
  mmgsdi_slot_data_type               * slot_data_ptr        = NULL;

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);
  memset(&session_cnf_info, 0x00, sizeof(mmgsdi_session_cnf_add_info_type));
  /*---------------------------------------------------------------------------
    Obtain index
    Index should be [0, MMGSDI_MAX_CLIENT_INFO-1],
    Index is set when
    a) a success queuing of command to UIM
    b) a failure in queuing command to UIM but client_request pointer should
      be set in a valid index location in mmgsdi process command
  ---------------------------------------------------------------------------*/
  index = uim_rsp_ptr->rsp_data_index;

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);

  mmgsdi_status = mmgsdi_util_get_client_request_table_info(
                                index,
                                &req_header,
                                &uim_cmd_ptr,
                                &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Error in getting info fr client req table info status=0x%x",
                  mmgsdi_status);
    return mmgsdi_status;
  }

  /* If dummy confirmation */
  if((extra_param_ptr != NULL) && (extra_param_ptr->close_data.is_dummy == TRUE))
  {
    return MMGSDI_SUCCESS;
  }

  if(!uim_rsp_ptr->is_uim_success)
  {
    /* mmgsdi status will be populated as a result of sw1, and sw2
       This will be passed into the build confirmation function */
   return mmgsdi_uicc_rsp_parse_uim_response(uim_rsp_ptr);
  }

  mmgsdi_status = mmgsdi_util_get_session_and_channel_info(
                     req_header.session_id,&session_info_ptr,&channel_info_ptr);

  if((mmgsdi_status != MMGSDI_SUCCESS) || (session_info_ptr == NULL) ||
     (channel_info_ptr == NULL))
  {
    return mmgsdi_cnf_build_and_queue(mmgsdi_status,
                                      &req_header,
                                      NULL,
                                      NULL,
                                      FALSE, /* sw not required */
                                      uim_rsp_ptr->status_word);
  }

  if(channel_info_ptr->app_info_ptr != NULL)
  {
    mmgsdi_memscpy((void *)&session_cnf_info.app_info,
                   sizeof(session_cnf_info.app_info),
                   (void *)&channel_info_ptr->app_info_ptr->app_data,
                   sizeof(mmgsdi_aid_type));
  }
  if(req_header.orig_request_type == MMGSDI_SESSION_OPEN_REQ ||
     req_header.orig_request_type == MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_REQ)
  {
    int32 client_index        = MMGSDI_MAX_CLIENT_INFO;
    int32 session_index       = MMGSDI_MAX_SESSION_INFO;

    MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

    /* we are here due to close channel for non prov session open failure */
    if(mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->num_connections > 1)
    {
      mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->num_connections--;
    }
    else if(mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->num_connections == 1 )
    {
      slot_data_ptr = mmgsdi_util_get_slot_data_ptr(
                        mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->slot_id);
      if(slot_data_ptr != NULL)
      {
        slot_data_ptr->available_logical_channels++;
      }

      MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]);
    }
    session_info_ptr->channel_info_index = MMGSDI_MAX_CHANNEL_INFO;

    mmgsdi_status = mmgsdi_util_get_client_and_session_index(
                      req_header.session_id,
                      &client_index,
                      &session_index);
    if(mmgsdi_status == MMGSDI_SUCCESS)
    {
      /* Dis-associate session from client */
      mmgsdi_client_id_reg_table[client_index]->num_session--;
      MMGSDIUTIL_TMC_MEM_FREE(session_info_ptr);
      mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index]
        = NULL;
    }

    /* since select aid failed set status as MMGSDI_ERROR */
    mmgsdi_status = MMGSDI_ERROR;
    MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;
  }
  else
  {
    /* Release session info */
    mmgsdi_status = mmgsdi_session_release_info(session_info_ptr,
                                                req_header.orig_request_type);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      /* Release was not completed */
      UIM_MSG_ERR_1("Fail to cleanup session_id=0x%x", req_header.session_id);
    }
  }

  UIM_MSG_HIGH_2("SESSION_RSP - CLOSE  status 0x%x orig_cmd=0x%x",
                 mmgsdi_status, req_header.orig_request_type);

  return mmgsdi_cnf_build_and_queue(mmgsdi_status, &req_header, NULL,
                                    &session_cnf_info, FALSE,
                                    uim_rsp_ptr->status_word);
} /* mmgsdi_session_rsp_close */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_RSP_DEACTIVATE

   DESCRIPTION:
     This function handles the response for Deactivating an application via
     a non-0 channel.  It also builds the confirmation message and put it
     onto the MMGSDI task queue.

   DEPENDENCIES:


   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                              but the service table indicates the card does not
                              have the support

   SIDE EFFECTS:
     None

   CRITICAL SECTIONS:
     The function uses mmgsdi_client_app_data_crit_sect_ptr to protect accesses
     to session info ptr and channel info ptr.
-----------------------------------------------------------------------------*/
static mmgsdi_return_enum_type mmgsdi_session_rsp_deactivate (
  const mmgsdi_uim_report_rsp_type *uim_rsp_ptr )
{
  mmgsdi_request_header_type           req_header           = {0};
  mmgsdi_return_enum_type              mmgsdi_status        = MMGSDI_SUCCESS;
  int32                                index                = 0;
  uim_cmd_type                       * uim_cmd_ptr          = NULL;
  mmgsdi_client_req_extra_info_type  * extra_param_ptr      = NULL;
  mmgsdi_return_enum_type              report_status        = MMGSDI_SUCCESS;
  mmgsdi_int_app_info_type           * app_info_ptr         = NULL;
  mmgsdi_channel_info_type           * channel_info_ptr     = NULL;
  mmgsdi_session_info_type           * session_info_ptr     = NULL;
  mmgsdi_slot_data_type              * slot_data_ptr        = NULL;
  mmgsdi_int_app_info_type           **app_info_tbl_pptr    = NULL;
  mmgsdi_session_cnf_add_info_type     session_cnf_info;

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);
  memset(&session_cnf_info, 0x00, sizeof(mmgsdi_session_cnf_add_info_type));

  index = uim_rsp_ptr->rsp_data_index;

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);

  mmgsdi_status = mmgsdi_util_get_client_request_table_info(
                                index,
                                &req_header,
                                &uim_cmd_ptr,
                                &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_DEBUG_MSG_ERROR_0("Error in getting info fr client req table info");
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_get_session_and_channel_info(
                     req_header.session_id,&session_info_ptr,&channel_info_ptr);

  if((mmgsdi_status != MMGSDI_SUCCESS) || (session_info_ptr == NULL) ||
     (channel_info_ptr == NULL))
  {
    UIM_MSG_HIGH_1("SESSION_RSP - DEACTIVATE  status 0x%x", mmgsdi_status);
    return mmgsdi_cnf_build_and_queue(mmgsdi_status,
                                      &req_header,
                                      NULL,
                                      NULL,
                                      FALSE, /* sw not required */
                                      uim_rsp_ptr->status_word);
  }

  if (uim_rsp_ptr->is_uim_success)
  {
    report_status = MMGSDI_SUCCESS;
  }
  else
  {
    /* mmgsdi status will be populated as a result of sw1, and sw2
       This will be passed into the build confirmation function */
    report_status = mmgsdi_uicc_rsp_parse_uim_response(uim_rsp_ptr);
  }

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(channel_info_ptr->slot_id);
  app_info_tbl_pptr = mmgsdi_util_get_app_pin_info_tbl_ptr(channel_info_ptr->slot_id);
  if((slot_data_ptr == NULL) || (app_info_tbl_pptr == NULL))
  {
    return mmgsdi_cnf_build_and_queue(MMGSDI_ERROR,
                                       &req_header,
                                       NULL,
                                       NULL,
                                       FALSE, /* sw not required */
                                       uim_rsp_ptr->status_word);
  }

  if (req_header.orig_request_type == MMGSDI_SESSION_CLOSE_REQ)
  {
    if(channel_info_ptr->app_info_ptr != NULL)
    {
      mmgsdi_memscpy((void *)&session_cnf_info.app_info,
                     sizeof(session_cnf_info.app_info),
                     (void *)&channel_info_ptr->app_info_ptr->app_data,
                     sizeof(mmgsdi_aid_type));
    }

    UIM_MSG_HIGH_2("SESSION_RSP DEACTIVATE report_status 0x%x, mmgsdi_status 0x%x",
                   report_status, mmgsdi_status);
    return mmgsdi_cnf_build_and_queue(report_status, &req_header, NULL,
                                      &session_cnf_info, FALSE,
                                      uim_rsp_ptr->status_word);
  }
  else if (req_header.orig_request_type == MMGSDI_SESSION_DEACTIVATE_REQ)
  {
    if(report_status == MMGSDI_SUCCESS)
    {
      /* De-activate can be called only for provisioned apps */
      mmgsdi_status = mmgsdi_util_get_prov_session_info(req_header.session_id,
                                                        NULL,
                                                        NULL,
                                                        &app_info_ptr);
      if((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
      {
        UIM_MSG_ERR_2("Error in obtaining provioned app status=0x%x, app_info_ptr=0x%x",
                      mmgsdi_status, app_info_ptr);
        return mmgsdi_cnf_build_and_queue(mmgsdi_status, &req_header, NULL,
                                          NULL, FALSE,
                                          uim_rsp_ptr->status_word);
      }

      mmgsdi_memscpy((void*)&session_cnf_info.app_info,
       sizeof(session_cnf_info.app_info),
       (void*)&app_info_ptr->app_data,
       sizeof(mmgsdi_aid_type));

      /* If the App. being deactivated is on a non-zero channel and termination
         of that app was not skipped earlier */
      if(channel_info_ptr->channel_id != MMGSDI_DEFAULT_CHANNEL &&
         !channel_info_ptr->app_termination_skipped)
      {
        /* extra_param_ptr is NULL, indicating that this is the AID deactivate confirmation,
           hence issue a MANAGE CHANNEL request */
        if(extra_param_ptr == NULL)
        {
          /* Extra parameters for the Session deactivate request */
          mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
                            MMGSDI_SESSION_DEACTIVATE_REQ,
                            (void*)&req_header,
                            &extra_param_ptr);

          if (mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
          {
            UIM_MSG_ERR_1("Unable to populate extra param 0x%x", mmgsdi_status);
            return mmgsdi_status;
          }
          mmgsdi_status = mmgsdi_uim_uicc_close_channel(&req_header,extra_param_ptr,
                                                        FALSE, NULL, NULL, MMGSDI_MAX_CHANNELS);
          /* In error cases, extra_param_ptr is freed by mmgsdi_uim_uicc_close_channel() */
          return mmgsdi_status;
        }
      } /* channel_id != DEFAULT CHANNEL */

      /* Cleanup Sessions linked to the provisioning app and notify them */
      mmgsdi_util_prov_session_deactivate_cleanup_and_notify(req_header.session_id);
    }

    return mmgsdi_cnf_build_and_queue(report_status, &req_header, NULL,
                                      (void*)&session_cnf_info, FALSE,
                                      uim_rsp_ptr->status_word);
  } /* else if (req_header.orig_request_type == MMGSDI_SESSION_DEACTIVATE_REQ)*/

  else if (req_header.orig_request_type == MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ)
  {
    if (report_status == MMGSDI_SUCCESS)
    {
      /* Build select aid command message */
      mmgsdi_status = mmgsdi_session_build_select_aid(
                        req_header.client_id,
                        req_header.session_id,
                        req_header.slot_id,
                        req_header.orig_request_type,
                        MMGSDI_SESSION_SELECT_AID_REQ,
                        channel_info_ptr->app_info_ptr->app_data.aid,
                        FALSE,
                        MMGSDI_SELECT_RSP_FCP,
                        req_header.response_cb,
                        req_header.client_data);

      if (mmgsdi_status == MMGSDI_SUCCESS)
      {
        return mmgsdi_status;
      }
      /* If not able to successfully build the aid req, send the error cnf */
    }
    UIM_MSG_HIGH_2("SESSION_RSP DEACTIVATE ERROR report_status 0x%x, mmgsdi_status 0x%x",
               report_status, mmgsdi_status);
    return mmgsdi_cnf_build_and_queue(MMGSDI_ERROR, &req_header, NULL,
                                      NULL, FALSE,
                                      uim_rsp_ptr->status_word);
  }
  else if (req_header.orig_request_type == MMGSDI_SESSION_APP_RESET_DEACTIVATE_REQ)
  {
    MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;
    if(channel_info_ptr->app_info_ptr != NULL)
    {
      mmgsdi_memscpy((void *)&session_cnf_info.app_info,
                     sizeof(session_cnf_info.app_info),
                     (void *)&channel_info_ptr->app_info_ptr->app_data,
                     sizeof(mmgsdi_aid_type));

      /* Cleanup PIN info connected to the app */
      mmgsdi_util_app_pin_cleanup(channel_info_ptr->app_info_ptr,
                                  channel_info_ptr->slot_id);

      /* Reset app and Perso states */
      if(channel_info_ptr->app_info_ptr->app_state > MMGSDI_APP_STATE_UNKNOWN)
      {
        channel_info_ptr->app_info_ptr->app_state =
          MMGSDI_APP_STATE_DETECTED;
      }
      channel_info_ptr->app_info_ptr->app_selected = FALSE;

      channel_info_ptr->app_info_ptr->perso_state = MMGSDI_SIMLOCK_NOT_VERIFIED;

      (void)mmgsdi_util_clear_app_capabilities(&channel_info_ptr->app_info_ptr->app_capabilities);
    }

    MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;
    return mmgsdi_cnf_build_and_queue(report_status, &req_header, NULL,
                                      (void*)&session_cnf_info, FALSE,
                                      uim_rsp_ptr->status_word);
  } /* else if (req_header.orig_request_type == MMGSDI_SESSION_APP_RESET_DEACTIVATE_REQ)*/
  else
  {
    /* can't build a "valid" confirmation from the context of this function,
       return to the process_response so that the static_cnf can be used to
       build a generic confirmation */
    UIM_MSG_ERR_1("Unknown orig_req in deactivate_rsp 0x%x",
                  req_header.orig_request_type);
    return MMGSDI_ERROR;
  }
} /* mmgsdi_session_rsp_deactivate*/


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_RSP_ACT_OR_SWITCH_PROV

   DESCRIPTION:
     This function handles the response for Deactivating an application via
     a non-0 channel.  It also builds the confirmation message and put it
     onto the MMGSDI task queue.

   DEPENDENCIES:


   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                              but the service table indicates the card does not
                              have the support

   SIDE EFFECTS:

-----------------------------------------------------------------------------*/
static mmgsdi_return_enum_type mmgsdi_session_rsp_act_or_switch_prov (
  const mmgsdi_uim_report_rsp_type *uim_rsp_ptr )
{
  mmgsdi_request_header_type           req_header           = {0};
  mmgsdi_return_enum_type              mmgsdi_status        = MMGSDI_SUCCESS;
  int32                                index                = 0;
  uim_cmd_type                       * uim_cmd_ptr          = NULL;
  mmgsdi_client_req_extra_info_type  * extra_param_ptr      = NULL;
  mmgsdi_channel_info_type           * channel_info_ptr     = NULL;
  mmgsdi_int_app_info_type          ** app_pin_tbl_pptr     = NULL;

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  index = uim_rsp_ptr->rsp_data_index;

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);

  mmgsdi_status = mmgsdi_util_get_client_request_table_info(
                                index,
                                &req_header,
                                &uim_cmd_ptr,
                                &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
  {
    MMGSDIUTIL_DEBUG_MSG_ERROR_0("Error in getting info fr client req table info");
    return MMGSDI_ERROR;
  }

  if(!uim_rsp_ptr->is_uim_success)
  {
    /* mmgsdi status will be populated as a result of sw1, and sw2
       This will be passed into the build confirmation function */
    return mmgsdi_uicc_rsp_parse_uim_response(uim_rsp_ptr);
  }

  /* Obtain the app currently activated from the session info */
  mmgsdi_status = mmgsdi_util_get_session_and_channel_info(
                    req_header.session_id,NULL,&channel_info_ptr);

  if ((mmgsdi_status != MMGSDI_SUCCESS) || (channel_info_ptr == NULL))
  {
    UIM_MSG_ERR_2("Unable to deactivate app associated with session id 0x%x. status=0x%x",
                  req_header.session_id, mmgsdi_status);

    return mmgsdi_cnf_build_and_queue(MMGSDI_ERROR, &req_header,NULL,NULL,
                                      FALSE,uim_rsp_ptr->status_word);
  }

  mmgsdi_status =  mmgsdi_session_build_select_aid(
                     req_header.client_id,
                     req_header.session_id,
                     req_header.slot_id,
                     req_header.orig_request_type,
                     MMGSDI_SESSION_DEACTIVATE_REQ,
                     channel_info_ptr->app_info_ptr->app_data.aid,
                     FALSE,
                     MMGSDI_SELECT_RSP_FCP,
                     req_header.response_cb,
                     req_header.client_data);

  /* Deactivate request queued successfully. */
  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    /* Clearing the old app details */
    channel_info_ptr->app_info_ptr->app_state =
        MMGSDI_APP_STATE_DETECTED;
    channel_info_ptr->app_info_ptr->perso_state = MMGSDI_SIMLOCK_NOT_VERIFIED;
    channel_info_ptr->app_info_ptr->pin1_ptr = NULL;
    channel_info_ptr->app_info_ptr->universal_pin_ptr = NULL;

    app_pin_tbl_pptr = mmgsdi_util_get_app_pin_info_tbl_ptr(
                           req_header.slot_id);
    if((app_pin_tbl_pptr == NULL) ||
       (app_pin_tbl_pptr[extra_param_ptr->open_data.app_pin_index] == NULL))
    {
      return MMGSDI_ERROR;
    }

    /* Point the channel info to the new app */
    channel_info_ptr->app_info_ptr =
      app_pin_tbl_pptr[extra_param_ptr->open_data.app_pin_index];
  }
  return mmgsdi_status;
} /* mmgsdi_session_rsp_act_or_switch_prov*/


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_RSP_OPEN

   DESCRIPTION:
     This function check the return data from UIM in regard to the Manage
     Channel Open operation.  If the channel was open successfully, it
     builds the Select AID command, else it builds the confirmation type

   DEPENDENCIES:


   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                              but the service table indicates the card does not
                              have the support

   SIDE EFFECTS:
     None.

   CRITICAL SECTIONS:
     The function uses mmgsdi_client_app_data_crit_sect_ptr to protect accesses
     to session info ptr and channel info ptr.
-----------------------------------------------------------------------------*/
static mmgsdi_return_enum_type mmgsdi_session_rsp_open(
  const mmgsdi_uim_report_rsp_type *uim_rsp_ptr )
{
  mmgsdi_request_header_type            req_header;
  mmgsdi_return_enum_type               mmgsdi_status       = MMGSDI_SUCCESS;
  mmgsdi_return_enum_type               report_status       = MMGSDI_SUCCESS;
  int32                                 index               = 0;
  uim_cmd_type                        * uim_cmd_ptr         = NULL;
  mmgsdi_client_req_extra_info_type   * extra_param_ptr     = NULL;
  mmgsdi_session_info_type            * session_info_ptr    = NULL;
  int32                                 channel_index       = 0;
  mmgsdi_static_data_type               app_id_data         = {0};
  mmgsdi_int_app_info_type           ** app_pin_tbl_pptr    = NULL;
  uint32                                i = 0, j            = 0;
  mmgsdi_slot_data_type               * slot_data_ptr       = NULL;
  boolean                               is_icc              = FALSE;
  mmgsdi_session_cnf_add_info_type      session_cnf_info;
  mmgsdi_return_enum_type               cnf_status           = MMGSDI_ERROR;
  mmgsdi_sw_status_type                 sw_status;
  boolean                               select_rsp_req       = FALSE;
  mmgsdi_select_rsp_enum_type           select_rsp_type      = MMGSDI_SELECT_RSP_FCP;
  uint8                                 card_channel_index   = MMGSDI_MAX_CHANNEL_INFO;
  uint16                                channel_update_count = 0;
  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);
  memset(&session_cnf_info, 0x00, sizeof(mmgsdi_session_cnf_add_info_type));
   /*---------------------------------------------------------------------------
    Obtain index
    Index should be [0, MMGSDI_MAX_CLIENT_INFO-1],
    Index is set when
    a) a success queuing of command to UIM
    b) a failure in queuing command to UIM but client_request pointer should
      be set in a valid index location in mmgsdi process command
  ---------------------------------------------------------------------------*/
  index = uim_rsp_ptr->rsp_data_index;

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);

  mmgsdi_status = mmgsdi_util_get_client_request_table_info(
                    index,
                    &req_header,
                    &uim_cmd_ptr,
                    &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_DEBUG_MSG_ERROR_0("Error in getting info for client req table info");
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_get_session_app_info(req_header.session_id,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   &session_info_ptr,
                                                   NULL);

  if(mmgsdi_status != MMGSDI_SUCCESS ||
     session_info_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  if (uim_rsp_ptr->is_uim_success &&
      uim_rsp_ptr->rsp_data.data_ptr != NULL &&
      uim_rsp_ptr->rsp_data.data_len > 0)
  {
    mmgsdi_status = MMGSDI_ERROR;
    /* For non ICC card - If there is already an entry in the channel table
      for the particular channel id being used find the index and use the same
      entry
      For ICC card - Always create a new entry in channel table, for ICC we
      support only channel 0 and we need to map each application on a separate
      entry in channel table */

    slot_data_ptr = mmgsdi_util_get_slot_data_ptr(req_header.slot_id);
    if(slot_data_ptr == NULL)
    {
      return MMGSDI_ERROR;
    }

    mmgsdi_status = mmgsdi_util_is_card_icc(slot_data_ptr,&is_icc);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      return MMGSDI_ERROR;
    }

    /* Check whether session is already activated and assigned in MAIN TASK
       context while SLOT specific task waits for MANAGE CHANNEL response.
       If yes, then channel is not associated with session id.
       Note that session has been activated on another app whose termination
       was skipped earlier during deactivation. As app termination was
       skipped, there was no need to send MANAGE CHANNEL or SELECT AID
       command to card. Thus session activation was completed in MAIN TASK
       context */
    if(req_header.orig_request_type ==
         MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ &&
       mmgsdi_util_is_session_id_valid(req_header.session_id) ==
         MMGSDI_SESSION_ID_ASSIGNED_AND_ACTIVE)
    {
      if(uim_rsp_ptr->rsp_data.data_ptr[0] != MMGSDI_DEFAULT_CHANNEL)
      {
        req_header.session_id = MMGSDI_INVALID_SESSION_ID;
        req_header.request_type = MMGSDI_SESSION_CLOSE_REQ;

        /* Close non-default channel; providing channel ID explicitly in
           function call as channel is not associated with session id.
           After closing the channel, error response will be sent for
           activation request */
        return mmgsdi_uim_uicc_close_channel(&req_header, NULL, FALSE, NULL, NULL,
                                             uim_rsp_ptr->rsp_data.data_ptr[0]);
      }

      /* Default channel is used; send error response for activation
         request */
      return mmgsdi_cnf_build_and_queue(MMGSDI_ERROR,
                                        &req_header,
                                        NULL,
                                        &session_cnf_info,
                                        FALSE,
                                        uim_rsp_ptr->status_word);
    }

    /* Lets get or generate the corresponding entry in channel info table */
    mmgsdi_status = mmgsdi_util_create_channel_info_table_entry(
                      req_header.orig_request_type,
                      req_header.slot_id,
                      is_icc,
                      uim_rsp_ptr->rsp_data.data_ptr[0],
                      &channel_index);

    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_cnf_build_and_queue(MMGSDI_ERROR, &req_header, NULL, NULL,
                                        FALSE, uim_rsp_ptr->status_word);
    }

    /* Map the new channel table entry with the session */
    session_info_ptr->channel_info_index = (uint8)channel_index;

    app_pin_tbl_pptr = mmgsdi_util_get_app_pin_info_tbl_ptr(req_header.slot_id);

    if((req_header.orig_request_type ==
          MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ) ||
        (req_header.orig_request_type == MMGSDI_SESSION_OPEN_REQ) ||
        (req_header.orig_request_type == MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_REQ))
    {
      if((extra_param_ptr != NULL) &&
         ((extra_param_ptr->open_data.aid.aid.data_len >= 0) &&
          (extra_param_ptr->open_data.aid.aid.data_len <= MMGSDI_MAX_AID_LEN)))
      {
        mmgsdi_memscpy((void*)app_id_data.data_ptr,
          sizeof(app_id_data.data_ptr),
          (void*)extra_param_ptr->open_data.aid.aid.data_ptr,
          int32touint32(extra_param_ptr->open_data.aid.aid.data_len));
        app_id_data.data_len = extra_param_ptr->open_data.aid.aid.data_len;

        /* Exclusive channel is opened only for non-prov apps */
        if((req_header.orig_request_type == MMGSDI_SESSION_OPEN_REQ) ||
           (req_header.orig_request_type == MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_REQ))
        {
          mmgsdi_channel_info_ptr_table[channel_index]->exclusive_channel =
            extra_param_ptr->open_data.exclusive_channel;

          /* Pass on whether select response is requested */
          select_rsp_req  = extra_param_ptr->open_data.select_rsp_requested;
          select_rsp_type = extra_param_ptr->open_data.select_rsp_type;
        }
      }

      if(mmgsdi_util_is_prov_session(session_info_ptr->session_type))
      {
        MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

        /* This could be SIM/RUIM app trying to activate.
           Map an entry in app_pin_info_table and send response. */
        if (app_pin_tbl_pptr != NULL)
        {
          for(i=0; i<MMGSDI_MAX_EXT_APP_INFO; i++)
          {
            if(app_pin_tbl_pptr[i] == NULL)
            {
              continue;
            }
            if(((app_pin_tbl_pptr[i]->app_data.app_type == MMGSDI_APP_SIM) &&
               ((session_info_ptr->session_type == MMGSDI_GW_PROV_PRI_SESSION) ||
                (session_info_ptr->session_type == MMGSDI_GW_PROV_SEC_SESSION) ||
                (session_info_ptr->session_type == MMGSDI_GW_PROV_TER_SESSION)))
                                              ||
               ((app_pin_tbl_pptr[i]->app_data.app_type == MMGSDI_APP_RUIM) &&
               ((session_info_ptr->session_type == MMGSDI_1X_PROV_PRI_SESSION) ||
                (session_info_ptr->session_type == MMGSDI_1X_PROV_SEC_SESSION) ||
                (session_info_ptr->session_type == MMGSDI_1X_PROV_TER_SESSION))))
            {
              app_pin_tbl_pptr[i]->app_selected = TRUE;
              app_pin_tbl_pptr[i]->app_state = MMGSDI_APP_STATE_ACTIVATED;
              mmgsdi_channel_info_ptr_table[channel_index]->app_info_ptr =
                app_pin_tbl_pptr[i];
            }
          }
        }
        /* --------------------------------------------------------------
        Map other provisioning app sessions of same type to the new channel
         ---------------------------------------------------------------- */
        for(i=0; i<MMGSDI_MAX_CLIENT_INFO; i++)
        {
          if (mmgsdi_client_id_reg_table[i] == NULL)
          {
            continue;
          }

          for(j=0; j<MMGSDI_MAX_SESSION_INFO; j++)
          {
            if (mmgsdi_client_id_reg_table[i]->session_info_ptr[j] == NULL)
            {
              continue;
            }

            card_channel_index = mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->channel_info_index;
            if((channel_index < MMGSDI_MAX_CHANNEL_INFO) &&
               (mmgsdi_channel_info_ptr_table[channel_index] != NULL) &&
               (card_channel_index != channel_index))
            {
              if((mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->session_type ==
                 session_info_ptr->session_type)
                                                ||
                 ((mmgsdi_channel_info_ptr_table[channel_index]->slot_id == MMGSDI_SLOT_1) &&
                  (mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->session_type ==
                   MMGSDI_CARD_SESSION_SLOT_1) &&
                  (mmgsdi_channel_info_ptr_table[channel_index]->channel_id ==
                   MMGSDI_DEFAULT_CHANNEL))
                                                ||
                 ((mmgsdi_channel_info_ptr_table[channel_index]->slot_id == MMGSDI_SLOT_2) &&
                  (mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->session_type ==
                   MMGSDI_CARD_SESSION_SLOT_2) &&
                  (mmgsdi_channel_info_ptr_table[channel_index]->channel_id ==
                   MMGSDI_DEFAULT_CHANNEL))
                                                ||
                 ((mmgsdi_channel_info_ptr_table[channel_index]->slot_id == MMGSDI_SLOT_3) &&
                  (mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->session_type ==
                   MMGSDI_CARD_SESSION_SLOT_3) &&
                  (mmgsdi_channel_info_ptr_table[channel_index]->channel_id ==
                   MMGSDI_DEFAULT_CHANNEL)))
              {
                channel_update_count++;
                if((card_channel_index < MMGSDI_MAX_CHANNEL_INFO) &&
                   (mmgsdi_channel_info_ptr_table[card_channel_index] != NULL) &&
                   (mmgsdi_channel_info_ptr_table[card_channel_index]->num_connections > 0))
                {
                  mmgsdi_channel_info_ptr_table[card_channel_index]->num_connections--;
                }
                mmgsdi_channel_info_ptr_table[channel_index]->num_connections++;
                mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->channel_info_index = (uint8)channel_index;
              }
            }
          } /* for j<MAX_SESSION_INFO */
        } /* for i<MAX_CLIENT_INFO */

        UIM_MSG_HIGH_3("Number of Channel mappings updated = 0x%x for session_type = 0x%x and channel index = 0x%x",
                        channel_update_count, session_info_ptr->session_type, channel_index);

        if((session_info_ptr->channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
           (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index] != NULL) &&
           (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr != NULL))
        {
          mmgsdi_memscpy((void *)&session_cnf_info.app_info,
            sizeof(session_cnf_info.app_info),
            (void *)&mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->app_data,
            sizeof(mmgsdi_aid_type));
        }
        MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

        /* In case of ICC card, just queue session open response */
        if(is_icc)
        {
          UIM_MSG_HIGH_0("SESSION_RSP SESSION OPEN SUCCESS");
          return mmgsdi_cnf_build_and_queue(mmgsdi_status,
                                            &req_header,
                                            NULL,
                                            &session_cnf_info,
                                            FALSE, /* sw not required */
                                            uim_rsp_ptr->status_word);
        }
      }
    }

    /* We send select AID only if
       1. We received session open request from client for non_prov app OR
       2. If the original request was to activate or switch provisioning.
       No need to check for provisioning session type in case of SESSION_OPEN
       request because we return above after mapping the session to an
       existing channel. */
    if((req_header.orig_request_type == MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ) ||
       ( ((req_header.orig_request_type == MMGSDI_SESSION_OPEN_REQ) ||
            (req_header.orig_request_type == MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_REQ)) &&
          mmgsdi_util_is_non_prov_session(session_info_ptr->session_type)))
    {
      /* Build the Select AID message */
      mmgsdi_status = mmgsdi_session_build_select_aid_from_indices(&req_header,
                                                                   app_id_data,
                                                                   select_rsp_req,
                                                                   select_rsp_type);

      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        /* Unable to build a select AID request, send back a failure conf
           followed up session clean up and close channel */
        UIM_MSG_HIGH_1("After Manage Channel, Select AID command setup failed",
                       mmgsdi_status);
        sw_status.sw1 = 0xFF;
        sw_status.sw2 = 0xFF;
        if (extra_param_ptr != NULL)
        {
          mmgsdi_memscpy((void*)&session_cnf_info.app_info,
            sizeof(session_cnf_info.app_info),
            (void*)&extra_param_ptr->open_data.aid,
            sizeof(mmgsdi_aid_type));
          session_cnf_info.select_rsp_requested =
            extra_param_ptr->open_data.select_rsp_requested;
          session_cnf_info.select_rsp_type =
            extra_param_ptr->open_data.select_rsp_type;
        }
        cnf_status = mmgsdi_cnf_build_and_queue(MMGSDI_SELECT_AID_FAILED,
                                                &req_header,
                                                NULL,
                                                &session_cnf_info,
                                                FALSE,
                                                sw_status);

        /* Since client has been notified about the select AID failure
           call a dummy callback to wrap up the close channel request */
        req_header.response_cb = mmgsdi_session_close_channel_internal_cb;

        mmgsdi_status = mmgsdi_session_open_failed_cleanup(&req_header,
                                                           uim_rsp_ptr->is_uim_success,
                                                           MMGSDI_ERROR);
        if (mmgsdi_status != MMGSDI_SUCCESS)
        {
          UIM_MSG_ERR_1("Session Open rsp session cleanup failed,status=0x%x",
                        mmgsdi_status);
        }
        return cnf_status;
      }
    }
    /* Select with MF handling is complete once Logical channel is opened.
       Build CNF to client */
    else if(req_header.orig_request_type == MMGSDI_SESSION_OPEN_WITH_MF_REQ)
    {
      /* Set exclusive channel to TRUE */
      mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->exclusive_channel = TRUE;

      mmgsdi_status = mmgsdi_cnf_build_and_queue(MMGSDI_SUCCESS,
                                        &req_header,
                                        NULL,
                                        NULL,
                                        TRUE,
                                        uim_rsp_ptr->status_word);
    }
    return mmgsdi_status;
  } /* if (uim_rsp_ptr->is_uim_success)*/
  else
  {
   /* mmgsdi status will be populated as a result of sw1, and sw2
       This will be passed into the build confirmation function */
    report_status = mmgsdi_uicc_rsp_parse_uim_response( uim_rsp_ptr);

    if (extra_param_ptr != NULL)
    {
      mmgsdi_memscpy((void*)&session_cnf_info.app_info,
        sizeof(session_cnf_info.app_info),
        (void*)&extra_param_ptr->open_data.aid,
        sizeof(mmgsdi_aid_type));
      session_cnf_info.select_rsp_requested =
        extra_param_ptr->open_data.select_rsp_requested;
      session_cnf_info.select_rsp_type =
        extra_param_ptr->open_data.select_rsp_type;
    }

    if(req_header.orig_request_type == MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ ||
       ((req_header.orig_request_type == MMGSDI_SESSION_OPEN_REQ ||
         req_header.orig_request_type == MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_REQ  ||
         req_header.orig_request_type == MMGSDI_SESSION_OPEN_WITH_MF_REQ) &&
        mmgsdi_util_is_non_prov_session(session_info_ptr->session_type)))
    {
      UIM_MSG_ERR_2("Manage channel failed.Building CNF with sw1=0x%x,sw2=0x%x",
                    uim_rsp_ptr->status_word.sw1, uim_rsp_ptr->status_word.sw2);
      cnf_status = mmgsdi_cnf_build_and_queue(MMGSDI_MANAGE_CHANNEL_FAILED,
                                              &req_header,
                                              NULL,
                                              &session_cnf_info,
                                              TRUE,
                                              uim_rsp_ptr->status_word);
    }
    else
    {
      UIM_MSG_ERR_2("General Error.Building CNF with sw1=0x%x,sw2=0x%x",
                    uim_rsp_ptr->status_word.sw1, uim_rsp_ptr->status_word.sw2);
      cnf_status = mmgsdi_cnf_build_and_queue(MMGSDI_ERROR,
                                              &req_header,
                                              NULL,
                                              &session_cnf_info,
                                              TRUE,
                                              uim_rsp_ptr->status_word);
    }

    if (cnf_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_1("Unable to build conf after failed session open,status=0x%x",
                    cnf_status);
    }

    /* No need to perform session clean up if session is alreay activated */
    if(req_header.orig_request_type ==
         MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ &&
       mmgsdi_util_is_session_id_valid(req_header.session_id) ==
         MMGSDI_SESSION_ID_ASSIGNED_AND_ACTIVE)
    {
      return cnf_status;
    }

    /* Open was unsuccessful, clean up the session info */
    mmgsdi_status = mmgsdi_session_open_failed_cleanup(&req_header,
                                                       uim_rsp_ptr->is_uim_success,
                                                       report_status);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_1("Unable to cleanup after failed session open,status=0x%x",
                    mmgsdi_status);
    }

    return cnf_status;
  }
} /* mmgsdi_session_rsp_open */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_PROC_OPEN_EXT

   DESCRIPTION:
     This function will perform parameter checks, generate multiple session IDs
     as requested, save the data into the client reg table

   DEPENDENCIES:


   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None.

   CRITICAL SECTIONS:
     The function uses mmgsdi_client_app_data_crit_sect_ptr to protect accesses
     to client id reg table.
-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_session_proc_open_ext(
  mmgsdi_session_open_ext_req_type *msg_ptr
)
{
  mmgsdi_return_enum_type                mmgsdi_status           = MMGSDI_SUCCESS;
  mmgsdi_session_id_type                 temp_session_id         = 0;
  uint32                                 i                       = 0;
  uint32                                 j                       = 0;
  mmgsdi_client_id_type                  client_id               = 0;
  int32                                  client_index            = 0;
  int32                                  session_index           = 0;
  int32                                  session_curr_index      = 0;
  mmgsdi_session_info_type              *session_ptr             = NULL;
  mmgsdi_sw_status_type                  status_word             = {0};
  mmgsdi_session_info_type              *ch_zero_session_ptr     = NULL;
  mmgsdi_channel_info_type              *ch_zero_ch_info_ptr     = NULL;
  uint8                                  num_sessions_requested  = 0;
  uint8                                  num_sessions_opened     = 0;
  uint32                                 session_mask            = 0;
  mmgsdi_slot_id_enum_type               card_session_slot       = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_session_type_enum_type          session_type            = MMGSDI_MAX_SESSION_TYPE_ENUM;
  uint32                                 set_notify              = 0;
  uint32                                 current_session_mask    = 1;
  mmgsdi_session_open_ext_cnf_info_type  session_cnf_ext_info;

  MMGSDIUTIL_RETURN_IF_NULL_2(msg_ptr,
                              msg_ptr->request_header.response_cb);

  memset(&session_cnf_ext_info,
         0x00,
         sizeof(mmgsdi_session_open_ext_cnf_info_type));

 /* -----------------------------------------------------------
  1) Check for valid client ID
  2) Obtain client index
  -------------------------------------------------------------- */

  session_mask   = msg_ptr->session_type_mask;
  client_id      = msg_ptr->request_header.client_id;
  mmgsdi_status  = mmgsdi_util_get_client_id_index(client_id,
                                                   &client_index);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("mmgsdi_session_proc_open_ext Invalid client id = 0x%x",
                  client_id);
    return mmgsdi_status;
  }

  /* Avoid invalid bits */
  session_mask = session_mask & MMGSDI_SESSION_VALID_MASK;

  UIM_MSG_HIGH_2("MMGSDI_SESSION_PROC_OPEN_EXT session mask = 0x%x, set notify mask = 0x%x",
                 session_mask, msg_ptr->set_notify_mask);

  /* Calculating the no of sessions requested */
  while(session_mask)
  {
    if(session_mask & MMGSDI_SESSION_TYPE_LSB_MASK)
    {
      num_sessions_requested++;
    }
    session_mask = session_mask >> 1;
  }

  /* mmgsdi_util_get_client_id_index success guarantees valid client_id_reg_table entry */
  /* If the no. of sessions requested alongwith no. of sessions already opened by the client
     exceeds the maximum limit supported for a client, return error.
   */
  if ( num_sessions_requested == 0 ||
      (mmgsdi_client_id_reg_table[client_index]->num_session + num_sessions_requested >
       MMGSDI_MAX_SESSION_INFO))
  {
    UIM_MSG_ERR_2("Reached max sessions supported for client = 0x%x Num of opened sessions = 0x%x",
                  client_id,
                  mmgsdi_client_id_reg_table[client_index]->num_session);

    return MMGSDI_ERROR;
  }

  /*Memory allocation to session info for all requested sessions*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(session_cnf_ext_info.session_info_ptr,
    (sizeof(mmgsdi_session_open_info_type) * num_sessions_requested));
  if(session_cnf_ext_info.session_info_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* -----------------------------------------------------------
  1) Generate session ID
  2) Map Session ID and Channel Id
  3) Do the above for all session requests.
  -------------------------------------------------------------- */
  while(num_sessions_requested)
  {
    session_mask = msg_ptr->session_type_mask & current_session_mask;

    if(session_mask == 0)
    {
      current_session_mask <<= 1;
      continue;
    }

    set_notify = 0;

    switch(session_mask)
    {
      case MMGSDI_GW_PROV_PRI_SESSION_MASK:
        session_type = MMGSDI_GW_PROV_PRI_SESSION;
        set_notify = msg_ptr->set_notify_mask & MMGSDI_GW_PROV_PRI_SESSION_SET_NOTIFY_MASK;
        break;

      case MMGSDI_GW_PROV_SEC_SESSION_MASK:
        session_type = MMGSDI_GW_PROV_SEC_SESSION;
        set_notify = msg_ptr->set_notify_mask & MMGSDI_GW_PROV_SEC_SESSION_SET_NOTIFY_MASK;
        break;

      case MMGSDI_GW_PROV_TER_SESSION_MASK:
       session_type = MMGSDI_GW_PROV_TER_SESSION;
       set_notify = msg_ptr->set_notify_mask & MMGSDI_GW_PROV_TER_SESSION_SET_NOTIFY_MASK;
       break;

      case MMGSDI_1X_PROV_PRI_SESSION_MASK:
        session_type = MMGSDI_1X_PROV_PRI_SESSION;
        set_notify = msg_ptr->set_notify_mask & MMGSDI_1X_PROV_PRI_SESSION_SET_NOTIFY_MASK;
        break;

      case MMGSDI_1X_PROV_SEC_SESSION_MASK:
        session_type = MMGSDI_1X_PROV_SEC_SESSION;
        set_notify = msg_ptr->set_notify_mask & MMGSDI_1X_PROV_SEC_SESSION_SET_NOTIFY_MASK;
        break;

      case MMGSDI_1X_PROV_TER_SESSION_MASK:
        session_type = MMGSDI_1X_PROV_TER_SESSION;
        set_notify = msg_ptr->set_notify_mask & MMGSDI_1X_PROV_TER_SESSION_SET_NOTIFY_MASK;
        break;

      case MMGSDI_CARD_SESSION_SLOT_1_MASK:
        session_type = MMGSDI_CARD_SESSION_SLOT_1;
        break;

      case MMGSDI_CARD_SESSION_SLOT_2_MASK:
        session_type = MMGSDI_CARD_SESSION_SLOT_2;
        break;

      case MMGSDI_CARD_SESSION_SLOT_3_MASK:
        session_type = MMGSDI_CARD_SESSION_SLOT_3;
        break;

      default:
      /* Unexpected here ! */
        UIM_MSG_ERR_1("Invalid session-type bit specified 0x%x",
                      (uint32)session_mask);

        mmgsdi_session_open_ext_cleanup(&session_cnf_ext_info,
                                        num_sessions_opened);
        return MMGSDI_ERROR;
    }

    mmgsdi_status = mmgsdi_util_get_free_session_index(client_index,
                                                       &session_index);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_0("No free index available in session_info_ptr[]");
      mmgsdi_session_open_ext_cleanup(&session_cnf_ext_info,
                                      num_sessions_opened);
      return MMGSDI_ERROR;
    }

    MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;
    do
    {

      /* client index and session index used in following memory allocation are
         valid because those are retrieved by using
         mmgsdi_util_get_client_id_index() and
         mmgsdi_util_get_free_session_index() respectively. */
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
        mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index],
        sizeof(mmgsdi_session_info_type));

      if(mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index] == NULL)
      {
        mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
        break;
      }

      session_ptr =
        mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index];

      if(session_ptr == NULL)
      {
        UIM_MSG_ERR_1("Session Ptr available in session_info_ptr[]is NULL for client 0x%x",
                      client_id);
        mmgsdi_status = MMGSDI_ERROR;
        break;
      }

      mmgsdi_client_id_reg_table[client_index]->num_session++;

      /* Populate following entities in session record from req_ptr
         1. Session Id
         2. app_type
         3. session_evt_cb_ptr
         4. notify_init
         Assign complement of session_ptr as session id to avoid exposing
         pointers. Use least significant 32 bits to maintain legacy behavior. */
      session_ptr->session_id             = (~((uint32)session_ptr)) & 0xFFFFFFFF;
      session_ptr->session_type           = session_type;
      session_ptr->session_evt_cb_ptr     = msg_ptr->evt_cb_ptr;
      session_ptr->channel_info_index     = MMGSDI_MAX_CHANNEL_INFO;

      /* if evt callback is null, no need to set set_notify */
      if( msg_ptr->evt_cb_ptr != NULL && set_notify )
      {
        session_ptr->notify_init          = TRUE;
      }
      else
      {
        session_ptr->notify_init          = FALSE;
      }

      (session_cnf_ext_info.session_info_ptr[session_curr_index]).session_id =
        session_ptr->session_id;
      (session_cnf_ext_info.session_info_ptr[session_curr_index]).session_type =
        session_ptr->session_type;

      num_sessions_opened++;
      session_cnf_ext_info.num_sessions = num_sessions_opened;
    } while (0);

    MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      mmgsdi_session_open_ext_cleanup(&session_cnf_ext_info,
                                      num_sessions_opened);
      return mmgsdi_status;
    }

    UIM_MSG_HIGH_4("Session id = 0x%x, Session type = 0x%x, notify init = 0x%x, Event callback ptr = 0x%x",
                   session_ptr->session_id,
                   session_type,
                   session_ptr->notify_init,
                   session_ptr->session_evt_cb_ptr);

    if (mmgsdi_util_is_prov_session(session_ptr->session_type))
    {
      /* For session open on prov app we map session with the entry
      in mmgsdi_channel_info_ptr_table for prov GW/CDMA app. If there
      is no match found then the client is the first one to open
      session on prov app. If the mmgsdi_channel_info_ptr entry points
      to null after the loop then the application has not been
      activated */

      MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;
      for(i=0; i<MMGSDI_MAX_CLIENT_INFO; i++)
      {
        if (mmgsdi_client_id_reg_table[i] == NULL)
        {
          continue;
        }
        for(j=0; j<MMGSDI_MAX_SESSION_INFO; j++)
        {
          if(mmgsdi_client_id_reg_table[i]->session_info_ptr[j] != NULL &&
            (mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->session_type ==
            session_ptr->session_type))
          {
            session_ptr->channel_info_index =
              mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->channel_info_index;

            if((session_ptr->channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
               (mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index] != NULL))
            {
              mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index]->num_connections++;
              (session_cnf_ext_info.session_info_ptr[session_curr_index]).channel_id =
                mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index]->channel_id;
              (session_cnf_ext_info.session_info_ptr[session_curr_index]).slot_id =
                mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index]->slot_id;
            }
            else
            {
              (session_cnf_ext_info.session_info_ptr[session_curr_index]).channel_id =
                MMGSDI_DEFAULT_CHANNEL;
              (session_cnf_ext_info.session_info_ptr[session_curr_index]).slot_id =
                MMGSDI_MAX_SLOT_ID_ENUM;
            }

            i = MMGSDI_MAX_CLIENT_INFO;
            break;
          }
          /* if session_info_ptr != NULL */
        } /* for j<MAX_SESSION_INFO */
      } /* for i<MAX_CLIENT_INFO */

      MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

      /* Build Confirmation */
      if((session_ptr->channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
         (mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index] != NULL) &&
         (mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index]->app_info_ptr != NULL))
      {
        mmgsdi_memscpy((void *)&session_cnf_ext_info.session_info_ptr[session_curr_index].app_info,
          sizeof(session_cnf_ext_info.session_info_ptr[session_curr_index].app_info),
          (void *)&mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index]->app_info_ptr->app_data,
          sizeof(mmgsdi_aid_type));
      }

      session_curr_index++;
    }
    else if (mmgsdi_util_is_card_session(session_ptr->session_type))
    {
      if (session_ptr->session_type == MMGSDI_CARD_SESSION_SLOT_1)
      {
        card_session_slot    = MMGSDI_SLOT_1;
      }
      else if(session_ptr->session_type == MMGSDI_CARD_SESSION_SLOT_2)
      {
        card_session_slot    = MMGSDI_SLOT_2;
      }
      else
      {
        card_session_slot    = MMGSDI_SLOT_3;
      }

      /* Map to default channel if app is already provisioned */
      mmgsdi_status = mmgsdi_util_get_ch_zero_app(card_session_slot,
                                                  NULL,
                                                  &temp_session_id);

      if(mmgsdi_status == MMGSDI_SUCCESS)
      {
        mmgsdi_status = mmgsdi_util_get_session_and_channel_info(
                         temp_session_id,
                         &ch_zero_session_ptr,
                         &ch_zero_ch_info_ptr);

        if((mmgsdi_status == MMGSDI_SUCCESS) &&
           (ch_zero_session_ptr != NULL))
        {
          MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;
          session_ptr->channel_info_index = ch_zero_session_ptr->channel_info_index;
          if ((session_ptr->channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
              (mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index] != NULL))
          {
            mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index]->num_connections++;
          }
          MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;
        }
      }
      (session_cnf_ext_info.session_info_ptr[session_curr_index]).channel_id =
        MMGSDI_DEFAULT_CHANNEL;
      (session_cnf_ext_info.session_info_ptr[session_curr_index]).slot_id =
        card_session_slot;
      session_curr_index++;
      mmgsdi_status = MMGSDI_SUCCESS;
    }
    else
    {
      /* Unexpected here ! */
      UIM_MSG_ERR_1("Invalid session type specified 0x%x",
                    (uint32)session_ptr->session_type);
      mmgsdi_session_open_ext_cleanup(&session_cnf_ext_info, num_sessions_opened);
      return MMGSDI_ERROR;
    }
    num_sessions_requested--;
    current_session_mask <<= 1;
  }
  (void)mmgsdi_cnf_build_and_queue(mmgsdi_status,
                             &msg_ptr->request_header,
                             NULL,
                             &session_cnf_ext_info,
                             FALSE, /* sw not required */
                             status_word);

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK( session_cnf_ext_info.session_info_ptr);

  return MMGSDI_SUCCESS;
} /* mmgsdi_session_proc_open_ext */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_OPEN_EXT_CLEAN_UP

   DESCRIPTION:
     This function will perform the cleanup of all the previously opened
     sessions if any of the requested session fails to open  in
     MMGSDI_SESSION_OPEN_EXT command.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE: None

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
void mmgsdi_session_open_ext_cleanup(
  mmgsdi_session_open_ext_cnf_info_type *session_cnf_ext_info_ptr,
  uint8                                  num_sessions_opened)
{
  mmgsdi_session_id_type            session_id             = 0;
  int32                             session_index          = 0;
  int32                             client_index           = 0;
  uint8                             curr_session_index     = 0;
  mmgsdi_return_enum_type           mmgsdi_status          = MMGSDI_ERROR;

  UIM_MSG_HIGH_0("mmgsdi_session_open_ext_cleanup");

  if(session_cnf_ext_info_ptr != NULL &&
     session_cnf_ext_info_ptr->session_info_ptr != NULL &&
     num_sessions_opened <= MMGSDI_SESSION_MAX_PROV_SESSIONS)
  {
    for(curr_session_index = 0; curr_session_index < num_sessions_opened;
        curr_session_index++)
    {
      session_id =
        session_cnf_ext_info_ptr->session_info_ptr[curr_session_index].session_id;
      /* Get the client index and session index from session_id */
      mmgsdi_status = mmgsdi_util_get_client_and_session_index(session_id,
                                                               &client_index,
                                                               &session_index);

      /* Clean the session */
      if(mmgsdi_status == MMGSDI_SUCCESS)
      {
        (void)mmgsdi_session_release_info(
          mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index],
          MMGSDI_SESSION_CLOSE_REQ);
      }
      else
      {
        /* bad session_id, continue cleaning up */
        UIM_MSG_ERR_1("Invalid session ID 0x%x, continue to leave req in table",
                  session_id);
      }
    }
    MMGSDIUTIL_TMC_MEM_FREE( session_cnf_ext_info_ptr->session_info_ptr);
  }
  else
  {
    UIM_MSG_ERR_2("Num of opened sessions 0x%x session cnf ext info ptr 0x%x",
                  num_sessions_opened,
                  session_cnf_ext_info_ptr);
  }
} /* mmgsdi_session_open_ext_cleanup */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_PROC_OPEN

   DESCRIPTION:
     This function will perform parameter checks, generate session ID, save
     the data into the client reg table and proceed with the Manage Channel
     Open command to the card

   DEPENDENCIES:


   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                              but the service table indicates the card does not
                              have the support

   SIDE EFFECTS:
     None.

   CRITICAL SECTIONS:
     The function uses mmgsdi_client_app_data_crit_sect_ptr to protect accesses
     to client id reg table.
-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_session_proc_open(
  mmgsdi_session_open_req_type *msg_ptr
)
{
  mmgsdi_return_enum_type           mmgsdi_status       = MMGSDI_SUCCESS;
  mmgsdi_session_id_type            temp_session_id     = 0;
  uint32                            j,i                 = 0;
  int32                             client_index        = 0;
  int32                             session_index       = 0;
  mmgsdi_session_info_type         *session_ptr         = NULL;
  mmgsdi_sw_status_type             status_word         = {0};
  mmgsdi_session_info_type         *ch_zero_session_ptr = NULL;
  mmgsdi_channel_info_type         *ch_zero_ch_info_ptr = NULL;
  mmgsdi_aid_type                   aid;
  mmgsdi_session_cnf_add_info_type  session_cnf_info;
  mmgsdi_slot_data_type            *slot_data_ptr       = NULL;
  uint8                             reserved_channels   = 0;

  MMGSDIUTIL_RETURN_IF_NULL_2(msg_ptr,
                              msg_ptr->request_header.response_cb);

  memset(&aid,0x00, sizeof(mmgsdi_aid_type));
  memset(&session_cnf_info,0x00, sizeof(mmgsdi_session_cnf_add_info_type));

  session_cnf_info.select_rsp_requested = msg_ptr->select_rsp_requested;
  session_cnf_info.select_rsp_type      = msg_ptr->select_rsp_type;

  /* Non-prov. Sessions are only supported on UICC cards. Reject request
     if not a UICC card */
  if(mmgsdi_util_is_non_prov_session(msg_ptr->session_type))
  {
    /* Update slot ID based on non-prov. Session type */
    if (msg_ptr->session_type == MMGSDI_NON_PROV_SESSION_SLOT_1)
    {
      msg_ptr->request_header.slot_id = MMGSDI_SLOT_1;
    }
    else if(msg_ptr->session_type == MMGSDI_NON_PROV_SESSION_SLOT_2)
    {
      msg_ptr->request_header.slot_id = MMGSDI_SLOT_2;
    }
    else
    {
      msg_ptr->request_header.slot_id = MMGSDI_SLOT_3;
    }

    slot_data_ptr = mmgsdi_util_get_slot_data_ptr(msg_ptr->request_header.slot_id);

    do
    {
      if(slot_data_ptr == NULL)
      {
        mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
        break;
      }

#ifdef FEATURE_UIM_MULTI_PROFILE
      /* Don't check mmgsdi_state when
         1) Requested client id is MMGSDI and
         2) Profile info global is not NULL and
         3) AID provided matches with the AID stored in the profile info global */
      if(!(msg_ptr->request_header.client_id == mmgsdi_generic_data_ptr->client_id &&
           mmgsdi_profile_info_ptr != NULL &&
           mmgsdi_profile_info_ptr->aid.data_ptr != NULL &&
           msg_ptr->non_prov_app_data.app_id_data.data_ptr != NULL &&
           mmgsdi_profile_info_ptr->aid.data_len == msg_ptr->non_prov_app_data.app_id_data.data_len &&
           memcmp(msg_ptr->non_prov_app_data.app_id_data.data_ptr,
                  mmgsdi_profile_info_ptr->aid.data_ptr,
                  msg_ptr->non_prov_app_data.app_id_data.data_len) == 0))
#endif /* FEATURE_UIM_MULTI_PROFILE */
      {
        if(slot_data_ptr->mmgsdi_state != MMGSDI_STATE_CARD_INSERTED)
        {
          UIM_MSG_ERR_2("Invalid slot state: 0x%x for non-prov session on slot 0x%x",
                        slot_data_ptr->mmgsdi_state,
                        msg_ptr->request_header.slot_id);

          mmgsdi_status = MMGSDI_ERROR;
          break;
        }
      }

      if((mmgsdi_util_is_adf_shareable(&msg_ptr->non_prov_app_data.app_id_data,
                                      msg_ptr->request_header.slot_id) == FALSE) &&
        (msg_ptr->non_prov_app_data.exclusive_channel))
      {
        UIM_MSG_ERR_0("Application is already selected and not shareable");
        mmgsdi_status = MMGSDI_APP_NOT_SHAREABLE;
      }
    }while(0);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_cnf_build_and_queue(mmgsdi_status,
                                        &msg_ptr->request_header,
                                        NULL,
                                        &session_cnf_info,
                                        FALSE, /* sw not required */
                                        status_word);
    }

    /* Check if a logical channel needs to be reserved for basic channel */
    if (!mmgsdi_util_check_is_default_channel_in_use_uicc_apps(msg_ptr->request_header.slot_id))
    {
      reserved_channels++;
    }
    /* Check if a logical channel needs to be reserved for CSIM */
    if(mmgsdi_util_reserve_logical_channel_for_csim_app(msg_ptr->request_header.slot_id))
    {
      reserved_channels++;
    }
  }

  /* Generate Session ID and add to Client ID reg. table */
  session_ptr = mmgsdi_util_generate_and_add_session_to_client(
                  msg_ptr->request_header.client_id,
                  msg_ptr->session_type);

  if(session_ptr == NULL)
  {
    UIM_MSG_ERR_2("Could not add new Session to Client table for client id low = 0x%x high = 0x%x",
                  (msg_ptr->request_header.client_id & 0xFFFFFFFF),
                  (msg_ptr->request_header.client_id >> 32 & 0xFFFFFFFF));

    return mmgsdi_cnf_build_and_queue(MMGSDI_ERROR,
                                      &msg_ptr->request_header,
                                      NULL,
                                      &session_cnf_info,
                                      FALSE, /* sw not required */
                                      status_word);
  }

  mmgsdi_status = mmgsdi_util_get_client_and_session_index(
                      session_ptr->session_id,
                      &client_index,
                      &session_index);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_cnf_build_and_queue(MMGSDI_ERROR,
                                      &msg_ptr->request_header,
                                      NULL,
                                      &session_cnf_info,
                                      FALSE, /* sw not required */
                                      status_word);
  }

  /* Populate following entities in session record from req_ptr
     1. session_evt_cb_ptr
     2. notify_init */
  session_ptr->session_evt_cb_ptr     = msg_ptr->evt_cb_ptr;
  session_ptr->notify_init            = msg_ptr->set_notify;
  session_ptr->channel_info_index     = MMGSDI_MAX_CHANNEL_INFO;
  msg_ptr->request_header.session_id  = session_ptr->session_id;

  if (mmgsdi_util_is_prov_session(session_ptr->session_type))
  {
    /* For session open on prov app we map session with the entry
    in mmgsdi_channel_info_ptr_table for prov GW/CDMA app. If there
    is no match found then the client is the first one to open
    session on prov app. If the mmgsdi_channel_info_ptr entry points
    to null after the loop then the application has not been
    activated */

    MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;
    for(i=0; i<MMGSDI_MAX_CLIENT_INFO; i++)
    {
      if (mmgsdi_client_id_reg_table[i] == NULL)
      {
        continue;
      }

      for(j=0; j<MMGSDI_MAX_SESSION_INFO; j++)
      {
        if(mmgsdi_client_id_reg_table[i]->session_info_ptr[j] != NULL &&
          (mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->session_type ==
           session_ptr->session_type))
        {
          session_ptr->channel_info_index =
            mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->channel_info_index;

          if((session_ptr->channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
             (mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index] != NULL))
          {
            /* if app termination is skipped and we are opening a session for
               same session type we should not increase the num of connections.
               mmgsdi session clean up depends on num_connections field.
               num_connections will be incremented along with other sessions
               when the activation is received */
            if(!(mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index]->app_termination_skipped))
            {
              mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index]->num_connections++;
            }

            /*Since the slot id is abstracted from the client, hence not
              provided by the client in request headr,lets populate it here*/
            msg_ptr->request_header.slot_id =
              mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index]->slot_id;
          }
          i = MMGSDI_MAX_CLIENT_INFO;
          break;
        }/* if session_info_ptr != NULL */
      } /* for j<MAX_SESSION_INFO */
    } /* for i<MAX_CLIENT_INFO */

    MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

    /* Build Confirmation */
    if((session_ptr->channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
       (mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index] != NULL) &&
       (mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index]->app_info_ptr != NULL))
    {
      mmgsdi_memscpy((void *)&session_cnf_info.app_info,
        sizeof(session_cnf_info.app_info),
        (void *)&mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index]->app_info_ptr->app_data,
        sizeof(mmgsdi_aid_type));
    }
    return mmgsdi_cnf_build_and_queue(mmgsdi_status,
                                      &msg_ptr->request_header,
                                      NULL,
                                      &session_cnf_info,
                                      FALSE, /* sw not required */
                                      status_word);
  }

  /* For session open on non-prov app
    1) Check if the requested aid data is valid from mmgsdi_app_pin_table_ptr
    2) Map channel entry in session record with a matching aid in
       mmgsdi_channel_info_ptr_table. If no match is found then send
       MANAGE CHANNEL command to UIM */
  else if (mmgsdi_util_is_non_prov_session(session_ptr->session_type))
  {
    boolean                 is_sw_needed  = FALSE;
    mmgsdi_return_enum_type report_status = MMGSDI_SUCCESS;

    aid.aid.data_len = msg_ptr->non_prov_app_data.app_id_data.data_len;

    /* Copy AID data only if AID length is non-zero & AID pointer is valid */
    if(aid.aid.data_len > 0 &&
       msg_ptr->non_prov_app_data.app_id_data.data_ptr != NULL)
    {
      mmgsdi_memscpy((void*)aid.aid.data_ptr,
                     sizeof(aid.aid.data_ptr),
                     (void*)msg_ptr->non_prov_app_data.app_id_data.data_ptr,
                     int32touint32(aid.aid.data_len));
    }

    /* Determine the AID type from the AID value */
    aid.app_type =
      mmgsdi_util_determine_app_type_from_aid(&aid.aid);

    if(slot_data_ptr == NULL)
    {
      report_status = MMGSDI_INCORRECT_PARAMS;
    }

    /* Check if a SELECT on this non-zero AID failed earlier... If so, we
       should just return failure here and not re-attempt the MANAGE CHANNEL
       followed by a SELECT AID */
    if (slot_data_ptr &&
        aid.aid.data_len > 0)
    {
      mmgsdi_aid_select_failed_node_type    *temp_ptr    = NULL;
      boolean                                is_aram_aid = FALSE;

      temp_ptr = slot_data_ptr->aid_select_failed_ptr;

      while (temp_ptr != NULL)
      {
        if (MMGSDI_SUCCESS == mmgsdi_util_cmp_aid(&aid, &temp_ptr->app_info, FALSE) &&
            MMGSDI_SUCCESS == mmgsdi_util_is_aram_aid(&temp_ptr->app_info, &is_aram_aid) &&
            is_aram_aid)
        {
          /* Found an ARA-M AID on which a first SELECT AID failed earlier */
          UIM_MSG_ERR_0("SELECT on an AID that has failed at least once earlier, is not allowed");
          status_word = temp_ptr->sw;
          report_status = MMGSDI_SELECT_AID_FAILED;
          is_sw_needed = TRUE;
          break;
        }
        temp_ptr = temp_ptr->next_ptr;
      }
    }

    if (report_status != MMGSDI_SUCCESS)
    {
      mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status,
                                                 &msg_ptr->request_header,
                                                 NULL,
                                                 &session_cnf_info,
                                                 is_sw_needed,
                                                 status_word);

      /* Open was unsuccessful, clean up the session info */
      (void) mmgsdi_session_open_failed_cleanup(&msg_ptr->request_header,
                                                FALSE,
                                                report_status);
      return mmgsdi_status;
    }

    /* No need to check whether if the AID is in the EF-DIR.
       It is possible to open a session even if the AID is not present in the EF-DIR. */
    if(msg_ptr->non_prov_app_data.exclusive_channel)
    {
      if(slot_data_ptr->available_logical_channels > reserved_channels)
      {
        UIM_MSG_HIGH_2("Total logical channels: 0x%x, Available logical channels: 0x%x",
                   slot_data_ptr->parsed_atr.total_logical_channels,
                   slot_data_ptr->available_logical_channels);
        /* Open a channel with the card */
        return mmgsdi_uim_uicc_open_channel(
                  (const mmgsdi_request_header_type*)&msg_ptr->request_header,
                  &aid, TRUE, msg_ptr->select_rsp_requested,
                  msg_ptr->select_rsp_type);
      }
      else
      {
        /* No available logical channels.  Build a failure response
           without actually sending command to the card. */
        UIM_MSG_ERR_0("No available logical channels for exclusive_channel Session Open Req");
        return mmgsdi_session_build_uim_open_channel_rsp(
                MMGSDI_DEFAULT_CHANNEL, /* channel id */
                msg_ptr->request_header,
                NULL,
                UIM_FAIL,
                msg_ptr->select_rsp_requested);
      }
    }
    /* Find matching entry for app in mmgsdi_channel_info_ptr_table */
    for(j=0; j<MMGSDI_MAX_CHANNEL_INFO; j++)
    {
      if(mmgsdi_channel_info_ptr_table[j]                    != NULL  &&
         mmgsdi_channel_info_ptr_table[j]->app_info_ptr      != NULL  &&
         mmgsdi_channel_info_ptr_table[j]->exclusive_channel == FALSE &&
         mmgsdi_channel_info_ptr_table[j]->slot_id           ==
           msg_ptr->request_header.slot_id                           &&
         mmgsdi_channel_info_ptr_table[j]->channel_state     !=
           MMGSDI_CHANNEL_STATE_NON_PROV_DEACT_IN_PROG)
      {
        if(mmgsdi_util_cmp_aid(
            &mmgsdi_channel_info_ptr_table[j]->app_info_ptr->app_data,
            &aid, FALSE)
            == MMGSDI_SUCCESS)
        {
          break;
        }
      } /* if app_info_ptr != NULL */
    } /* for j<MAX_CHANNEL_INFO */

    /* Requested app already open on card, map new Session to logical channel
       on which the app is open */
    if(j < MMGSDI_MAX_CHANNEL_INFO &&
       mmgsdi_channel_info_ptr_table[j] != NULL)
    {
      MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;
      session_ptr->channel_info_index = j;
      mmgsdi_channel_info_ptr_table[j]->channel_state =
        MMGSDI_CHANNEL_STATE_NON_PROV_ACT_IN_PROG;
      mmgsdi_channel_info_ptr_table[j]->num_connections++;
      MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

      /* Build Confirmation */
      if(mmgsdi_channel_info_ptr_table[j]->app_info_ptr != NULL)
      {
        mmgsdi_memscpy((void *)&session_cnf_info.app_info,
          sizeof(session_cnf_info.app_info),
          (void *)&mmgsdi_channel_info_ptr_table[j]->app_info_ptr->app_data,
          sizeof(mmgsdi_aid_type));
      }

      session_cnf_info.select_rsp_requested = msg_ptr->select_rsp_requested;
      session_cnf_info.select_rsp_type = msg_ptr->select_rsp_type;

      mmgsdi_status =  mmgsdi_cnf_build_and_queue(MMGSDI_SUCCESS,
                                                  &msg_ptr->request_header,
                                                  NULL,
                                                  &session_cnf_info,
                                                  FALSE, // sw not required
                                                  status_word);
    }
    else
    {
      if(slot_data_ptr->available_logical_channels > reserved_channels)
      {
        /* Open a channel with the card */
        mmgsdi_status = mmgsdi_uim_uicc_open_channel(
                         (const mmgsdi_request_header_type*)&msg_ptr->request_header,
                         &aid, FALSE, msg_ptr->select_rsp_requested, msg_ptr->select_rsp_type);
      }
      else
      {
        /* No available logical channels.  Build a failure response
           without actually sending command to the card. */
        UIM_MSG_ERR_0("No available logical channels for exclusive_channel Session Open Req");
        return mmgsdi_session_build_uim_open_channel_rsp(
                MMGSDI_DEFAULT_CHANNEL, /* channel id */
                msg_ptr->request_header,
                NULL,
                UIM_FAIL,
                msg_ptr->select_rsp_requested);
      }
    }
  }
  else if (mmgsdi_util_is_card_session(session_ptr->session_type))
  {
    if (session_ptr->session_type == MMGSDI_CARD_SESSION_SLOT_1)
    {
      msg_ptr->request_header.slot_id = MMGSDI_SLOT_1;
    }
    else if(session_ptr->session_type == MMGSDI_CARD_SESSION_SLOT_2)
    {
      msg_ptr->request_header.slot_id = MMGSDI_SLOT_2;
    }
    else
    {
      msg_ptr->request_header.slot_id = MMGSDI_SLOT_3;
    }

    /* Map to default channel if app is already provisioned */
    mmgsdi_status = mmgsdi_util_get_ch_zero_app(
                      msg_ptr->request_header.slot_id,
                      NULL,
                      &temp_session_id);

    if(mmgsdi_status == MMGSDI_SUCCESS)
    {
      mmgsdi_status = mmgsdi_util_get_session_and_channel_info(
                        temp_session_id,
                        &ch_zero_session_ptr,
                        &ch_zero_ch_info_ptr);

      if((mmgsdi_status == MMGSDI_SUCCESS) &&
         (ch_zero_session_ptr != NULL))
      {
        MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;
        session_ptr->channel_info_index = ch_zero_session_ptr->channel_info_index;
        if((session_ptr->channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
           (mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index] != NULL))
        {
          mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index]->num_connections++;
        }
        MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;
      }
    }
    mmgsdi_status =
      mmgsdi_cnf_build_and_queue(MMGSDI_SUCCESS,&msg_ptr->request_header,
                                 NULL,&session_cnf_info,FALSE,status_word);
  }
  else
  {
    UIM_MSG_ERR_1("Invalid session type specified 0x%x",
                  (uint32)session_ptr->session_type);

    /* Delete the session record */
    mmgsdi_status = mmgsdi_util_get_client_and_session_index(
                      session_ptr->session_id,
                      &client_index,
                      &session_index);

    if(mmgsdi_status == MMGSDI_SUCCESS)
    {
      MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

      /* Delete the session record */
      if(mmgsdi_client_id_reg_table[client_index]->num_session > 0)
      {
        mmgsdi_client_id_reg_table[client_index]->num_session--;
      }
      mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index]
       = NULL;
      MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;
      MMGSDIUTIL_TMC_MEM_FREE(session_ptr);

      return mmgsdi_cnf_build_and_queue(MMGSDI_ERROR,
                                        &msg_ptr->request_header,
                                        NULL,
                                        &session_cnf_info,
                                        FALSE, /* sw not required */
                                        status_word);
    }
  }

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_cnf_build_and_queue(mmgsdi_status,
                                      &msg_ptr->request_header,
                                      NULL,
                                      &session_cnf_info,
                                      FALSE, /* sw not required */
                                      status_word);
  }
  return mmgsdi_status;
} /* mmgsdi_session_proc_open */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_PROC_OPEN_WITH_MF

   DESCRIPTION:
     This function will perform parameter checks, generate session ID, save
     the data into the client reg table and proceed with the Manage Channel
     Open command to the card

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_NOT_SUPPORTED:    Command not supported

   SIDE EFFECTS:
     None
-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_session_proc_open_with_mf(
  mmgsdi_session_open_with_mf_req_type *msg_ptr
)
{
  mmgsdi_session_info_type  *session_ptr       = NULL;
  mmgsdi_slot_data_type     *slot_data_ptr     = NULL;
  uint8                     reserved_channels  = 0;

  MMGSDIUTIL_RETURN_IF_NULL_2(msg_ptr,
                              msg_ptr->request_header.response_cb);

  /* Select with MF only supported for non-prov. sessions */
  if(!mmgsdi_util_is_non_prov_session(msg_ptr->session_type))
  {
    UIM_MSG_ERR_1("Invalid Session type: 0x%x for Session Open with MF",
                  msg_ptr->session_type);
    return MMGSDI_ERROR;
  }

  /* Get the slot data info */
  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(
                    msg_ptr->request_header.slot_id);

  if(slot_data_ptr == NULL)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* Check slot state */
  if(slot_data_ptr->mmgsdi_state != MMGSDI_STATE_CARD_INSERTED)
  {
    UIM_MSG_ERR_2("Invalid slot state: 0x%x for non-prov session on slot 0x%x",
                  slot_data_ptr->mmgsdi_state,
                  msg_ptr->request_header.slot_id);

    return MMGSDI_ERROR;
  }

  /* Generate Session ID and add to Client ID reg. table */
  session_ptr = mmgsdi_util_generate_and_add_session_to_client(
                    msg_ptr->request_header.client_id,
                    msg_ptr->session_type);

  if(session_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* Check if a logical channel needs to be reserved for basic channel */
  if (!mmgsdi_util_check_is_default_channel_in_use_uicc_apps(msg_ptr->request_header.slot_id))
  {
    reserved_channels++;
  }

   /* Check if a logical channel needs to be reserved for CSIM */
  if(mmgsdi_util_reserve_logical_channel_for_csim_app(msg_ptr->request_header.slot_id))
  {
    reserved_channels++;
  }

  /* Populate following entities in session record from req_ptr
     1. session_evt_cb_ptr
     2. notify_init */
  session_ptr->session_evt_cb_ptr    = msg_ptr->evt_cb_ptr;
  session_ptr->channel_info_index    = MMGSDI_MAX_CHANNEL_INFO;
  msg_ptr->request_header.session_id = session_ptr->session_id;

  /* Open a channel with the card */
  if(slot_data_ptr->available_logical_channels > reserved_channels)
  {
    return mmgsdi_uim_uicc_open_channel(
            (const mmgsdi_request_header_type*)&msg_ptr->request_header,
            NULL, TRUE, FALSE, MMGSDI_SELECT_RSP_NO_DATA);
  }
  else
  {
    UIM_MSG_ERR_0("No available logical channels for Session Open with MF request");
    return MMGSDI_ERROR;
  }
} /* mmgsdi_session_proc_open_with_mf */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_PROC_CLOSE

   DESCRIPTION:
     This function will perform parameter checksand proceed with the Deactivate OR
     Manage Channel Close command to the card depending on the case

     For Non Prov app
     1) Send select deactivate if no other non prov is sharing the channel
     2) If no more non prov is sharing the channel, after select deactivate, then
        also send close channel to card
        If there is still something shared, decrement the channel info's number of
        reference
     Both above cases require clean up of the session info table, delink it with
     any channel info ptr and also free that memory space also decrement the
     client id's num_session count

     For Prov app
      Clean up that particular session table + delink + decrement client num of
      session count

   DEPENDENCIES:
    None

   LIMITATIONS:
    None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                              but the service table indicates the card does not
                              have the support

   SIDE EFFECTS:
     None.

   CRITICAL SECTIONS:
     The function uses mmgsdi_client_app_data_crit_sect_ptr to protect accesses
     to session info ptr.
-------------------------------------------------------------------------------*/
static mmgsdi_return_enum_type mmgsdi_session_proc_close(
  mmgsdi_session_close_req_type *msg_ptr
)
{
  mmgsdi_session_info_type          *session_info_ptr      = NULL;
  mmgsdi_channel_info_type          *channel_info_ptr      = NULL;
  mmgsdi_return_enum_type            mmgsdi_status         = MMGSDI_ERROR;
  mmgsdi_sw_status_type              status_word           = {0};
  mmgsdi_session_cnf_add_info_type   session_cnf_info;
  int32                              client_index          = 0;
  int32                              client_dummy_index    = 0;
  mmgsdi_client_req_extra_info_type *extra_param_ptr       = NULL;
  mmgsdi_client_req_extra_info_type *extra_param_dummy_ptr = NULL;
  mmgsdi_slot_data_type             *slot_data_ptr         = NULL;

  memset(&session_cnf_info, 0x00, sizeof(mmgsdi_session_cnf_add_info_type));

  MMGSDIUTIL_RETURN_IF_NULL(msg_ptr);

  mmgsdi_status = mmgsdi_util_get_session_and_channel_info(
                    msg_ptr->request_header.session_id,
                    &session_info_ptr,
                    &channel_info_ptr);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }
  MMGSDIUTIL_RETURN_STATUS_IF_NULL(session_info_ptr, MMGSDI_ERROR);
  /* In case where session_close request will come immediate after
     the session_open request, channel_info_ptr will never set
     and it points to NULL. */
  if((NULL != channel_info_ptr) && (NULL != channel_info_ptr->app_info_ptr))
  {
    mmgsdi_memscpy((void *)&session_cnf_info.app_info,
      sizeof(session_cnf_info.app_info),
      (void *)&channel_info_ptr->app_info_ptr->app_data,
      sizeof(mmgsdi_aid_type));
  }

  /* Populate the Slot ID so it can be returned in the CNF */
  if(NULL != channel_info_ptr)
  {
    msg_ptr->request_header.slot_id = channel_info_ptr->slot_id;
  }

  if (mmgsdi_util_is_prov_session(session_info_ptr->session_type))
  {
    /* If activation of provisioning failed, then close any
       channel opened for the application */
    if((msg_ptr->request_header.orig_request_type ==
        MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ) &&
       (channel_info_ptr != NULL) &&
       (channel_info_ptr->channel_id != MMGSDI_DEFAULT_CHANNEL))
    {
      /* we are here because select aid for provisioning activation failed */
      return mmgsdi_uim_uicc_close_channel(&msg_ptr->request_header, NULL,
                                           FALSE, NULL, NULL, MMGSDI_MAX_CHANNELS);
    }
    else
    {
      mmgsdi_status = mmgsdi_session_release_info(
                        session_info_ptr,
                        msg_ptr->request_header.orig_request_type);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        return mmgsdi_status;
      }
      /* build confirmation and send it to the client. Client should
         not expect SW for session close on prov app  */
      mmgsdi_status = mmgsdi_cnf_build_and_queue(mmgsdi_status,
                                                 &msg_ptr->request_header,
                                                 NULL,
                                                 &session_cnf_info,
                                                 FALSE,
                                                 status_word);
    }
  }
  else if (mmgsdi_util_is_non_prov_session(session_info_ptr->session_type))
  {
    /* Handle Session close request issued due to a failed Session Open or
       a Session Close request for a Session Opened through Select with MF */
    if((msg_ptr->request_header.orig_request_type == MMGSDI_SESSION_OPEN_REQ) ||
       (msg_ptr->request_header.orig_request_type == MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_REQ)||
       ((msg_ptr->request_header.orig_request_type == MMGSDI_SESSION_CLOSE_REQ) &&
        (channel_info_ptr != NULL) &&
        (channel_info_ptr->app_info_ptr == NULL)))
    {
      /* Close the opened logical channel */
      if((channel_info_ptr != NULL) &&
         (channel_info_ptr->channel_id != MMGSDI_DEFAULT_CHANNEL))
      {
        MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

        /*set state of channel to DEACTIVATION IN PROGRESS*/
        channel_info_ptr->channel_state =
          MMGSDI_CHANNEL_STATE_NON_PROV_DEACT_IN_PROG;

        MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

        return mmgsdi_uim_uicc_close_channel(&msg_ptr->request_header, NULL,
                                             FALSE, NULL, NULL, MMGSDI_MAX_CHANNELS);
      }
      else
      {
        int32 session_index       = MMGSDI_MAX_SESSION_INFO;

        MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

        UIM_MSG_ERR_0("Non-prov. session should not have been opened on default channel");

        if (channel_info_ptr != NULL)
        {
          if(channel_info_ptr->num_connections > 1)
          {
            channel_info_ptr->num_connections--;
          }
          else if(channel_info_ptr->num_connections == 1)
          {
            slot_data_ptr = mmgsdi_util_get_slot_data_ptr(channel_info_ptr->slot_id);
            if(slot_data_ptr != NULL)
            {
              slot_data_ptr->available_logical_channels++;
            }

            MMGSDIUTIL_TMC_MEM_FREE(channel_info_ptr);
          }
        }
        session_info_ptr->channel_info_index = MMGSDI_MAX_CHANNEL_INFO;

        mmgsdi_status = mmgsdi_util_get_client_and_session_index(
                          msg_ptr->request_header.session_id,
                          &client_index,
                          &session_index);
        if(mmgsdi_status == MMGSDI_SUCCESS)
        {
          /* Dis-associate session from client */
          mmgsdi_client_id_reg_table[client_index]->num_session--;
          MMGSDIUTIL_TMC_MEM_FREE(session_info_ptr);
          mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index]
            = NULL;
        }
        MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

        /* build confirmation and send it to the client  */
        return mmgsdi_cnf_build_and_queue(MMGSDI_ERROR,
                                         &msg_ptr->request_header,
                                          NULL,
                                          NULL,
                                          FALSE,
                                          status_word);
      }
    }

    /* If Non-prov session is opened successfully,
       then it shall always have channel information with it.
       num_connections is increased while opening the session when app is
       already deactivated but termination is skipped; it is required to
       release session info */
    MMGSDIUTIL_RETURN_STATUS_IF_NULL(channel_info_ptr, MMGSDI_ERROR);
    if(channel_info_ptr->num_connections > 1 || channel_info_ptr->app_termination_skipped)
    {
      mmgsdi_status = mmgsdi_session_release_info(
                        session_info_ptr,
                        msg_ptr->request_header.orig_request_type);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        return mmgsdi_status;
      }
      /* build confirmation and send it to the client  */
      mmgsdi_status = mmgsdi_cnf_build_and_queue(mmgsdi_status,
                                                 &msg_ptr->request_header,
                                                 NULL,
                                                 &session_cnf_info,
                                                 FALSE,
                                                 status_word);
    }
    /* channel is not shared by anyone else */
    else if(channel_info_ptr->num_connections == 1)
    {
      MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

      /* set state of channel to DEACTIVATION IN PROGRESS */
      channel_info_ptr->channel_state =
        MMGSDI_CHANNEL_STATE_NON_PROV_DEACT_IN_PROG;

      /* set select response type to RSP NO DATA */
      channel_info_ptr->select_rsp_type =
        MMGSDI_SELECT_RSP_NO_DATA;

      /* Populate the Slot ID corresponding to the app */
      msg_ptr->request_header.slot_id = channel_info_ptr->slot_id;

      MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

      /* Extra parameters for the Session close request */
      mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
                        MMGSDI_SESSION_CLOSE_REQ,
                        (void*)msg_ptr,
                        &extra_param_ptr);

      if (mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
      {
        UIM_MSG_ERR_1("Unable to populate extra param 0x%x", mmgsdi_status);
        return mmgsdi_status;
      }

      /* Deactivate if application can handle deactivation
         Client to determine if termination is appropriate */
      if(msg_ptr->non_prov_terminate)
      {
        /* Get client index to store the original request details */
        mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&client_index);
        if (mmgsdi_status != MMGSDI_SUCCESS)
        {
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
          return mmgsdi_status;
        }
        mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                          client_index,
                          &msg_ptr->request_header,
                          NULL,
                          extra_param_ptr);

        if (mmgsdi_status != MMGSDI_SUCCESS)
        {
          mmgsdi_util_free_client_request_table_index(client_index);
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
          return mmgsdi_status;
        }
        /* Queue a Deactivate request to MMGSDI */
        mmgsdi_status = mmgsdi_session_build_deactivate(
                          msg_ptr->request_header.client_id,
                          msg_ptr->request_header.session_id,
                          msg_ptr->request_header.slot_id,
                          msg_ptr->request_header.orig_request_type,
                          mmgsdi_session_internal_deact_cb,
                          (mmgsdi_client_data_type)client_index);
      }
      else
      {
        /* Close channel without deactivating application */
        mmgsdi_status = mmgsdi_uim_uicc_close_channel(&msg_ptr->request_header,
                                                      extra_param_ptr,
                                                      FALSE, NULL, NULL,
                                                      MMGSDI_MAX_CHANNELS);
      }

      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        if (msg_ptr->non_prov_terminate)
        {
          mmgsdi_util_free_client_request_table_index(client_index);
        }
        UIM_MSG_ERR_1("Unable to close channel/deactivate app 0x%x", mmgsdi_status);
        return mmgsdi_status;
      }

      /* Get client index to store the dummy Session close response details */
      mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&client_dummy_index);
      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        return mmgsdi_status;
      }

      mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
                MMGSDI_SESSION_CLOSE_REQ,
                (void*)msg_ptr,
                &extra_param_dummy_ptr);

      if (mmgsdi_status != MMGSDI_SUCCESS || extra_param_dummy_ptr == NULL)
      {
        UIM_MSG_ERR_1("Unable to populate extra param 0x%x", mmgsdi_status);
        mmgsdi_util_free_client_request_table_index(client_dummy_index);
        return mmgsdi_status;
      }

      /* This is the dummy Session Close response */
      extra_param_dummy_ptr->close_data.is_dummy = TRUE;

      mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                        client_dummy_index,
                        &msg_ptr->request_header,
                        NULL,
                        extra_param_dummy_ptr);

      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        mmgsdi_util_free_client_request_table_index(client_dummy_index);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_dummy_ptr);
        return mmgsdi_status;
      }

      /* Queue the dummy Session close response to the MMGSDI task response queue.
         This is needed to kick-start MMGSDI to process commands on its command queue */
      mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(
                      client_dummy_index, MMGSDI_SESSION_CLOSE_REQ, MMGSDI_SUCCESS);

      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        mmgsdi_util_free_client_request_table_index(client_dummy_index);
        return mmgsdi_status;
      }
    }
  }
  else if (mmgsdi_util_is_card_session(session_info_ptr->session_type))
  {
    mmgsdi_status = mmgsdi_session_release_info(
                      session_info_ptr,
                      msg_ptr->request_header.orig_request_type);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }

    /* Build confirmation */
    mmgsdi_status = mmgsdi_cnf_build_and_queue(mmgsdi_status,
                                               &msg_ptr->request_header,
                                               NULL,
                                               NULL,
                                               FALSE,
                                               status_word);
  }
  else
  {
    UIM_MSG_ERR_1("Invalid session type 0x%x",session_info_ptr->session_type);
  }
  return mmgsdi_status;
} /* mmgsdi_session_proc_close */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_ACTIVATE_PROVISIONING_START

   DESCRIPTION:
     This function will verify if the activate provisioning can be started,
     and if so start the activation procedure

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:
     None
-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_session_activate_provisioning_start(
  mmgsdi_session_act_or_switch_prov_req_type *msg_ptr
)
{
  uim_cmd_type                      *uim_cmd_ptr          = NULL;
  mmgsdi_client_req_extra_info_type *extra_param_ptr      = NULL;
  boolean                            is_app_active        = FALSE;
  mmgsdi_session_type_enum_type      session_type         = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_session_type_enum_type      session_type_refresh = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_return_enum_type            mmgsdi_status        = MMGSDI_ERROR;
  uint8                              slot_index           = MMGSDI_SLOT_1_INDEX;
  mmgsdi_slot_data_type             *slot_data_ptr        = NULL;
  boolean                            is_icc_card          = FALSE;

  MMGSDIUTIL_RETURN_IF_NULL(msg_ptr);

  /* If a APP_RESET_AUTO REFRESH is in progress, verify that this is not due to
     a previous switch provisioning request on the same Session type */
  mmgsdi_status = mmgsdi_util_get_slot_index(msg_ptr->request_header.slot_id,
                                             &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }
  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    return MMGSDI_ERROR;
  }

  /* Determine the card type */
  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(msg_ptr->request_header.slot_id);
  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_is_card_icc(slot_data_ptr, &is_icc_card);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  if(mmgsdi_refresh_info_ptr[slot_index] != NULL)
  {
    if(mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_APP_RESET_AUTO)
    {
      mmgsdi_request_header_type req_header;

      /* Retrieve the original deact/switch request */
      mmgsdi_status = mmgsdi_util_get_client_request_table_info(
                        (int32) mmgsdi_refresh_info_ptr[slot_index]->request_header.client_data,
                        &req_header,
                        &uim_cmd_ptr,
                        &extra_param_ptr);

      if (mmgsdi_status == MMGSDI_SUCCESS)
      {
        if(req_header.orig_request_type == MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ)
        {
          /* Check if same session type is being activated */
          mmgsdi_status = mmgsdi_util_get_session_type(msg_ptr->request_header.session_id,
                                                       &session_type,
                                                       NULL);
          if(mmgsdi_status == MMGSDI_SUCCESS)
          {
            mmgsdi_status = mmgsdi_util_get_session_type(req_header.session_id,
                                                         &session_type_refresh,
                                                         NULL);

            if((mmgsdi_status == MMGSDI_SUCCESS) &&
               (session_type == session_type_refresh))
            {
              UIM_MSG_ERR_1("Session already being activated on slot 0x%x",
                            msg_ptr->request_header.slot_id);
              return MMGSDI_ERROR;
            }
          }

          /* Check if same UICC app id is being activated */
          if((msg_ptr->app_id_data.data_len > 0) &&
             (msg_ptr->app_id_data.data_len <= MMGSDI_MAX_AID_LEN))
          {
            if((mmgsdi_refresh_info_ptr[slot_index]->aid.aid.data_len ==
                    msg_ptr->app_id_data.data_len) &&
               (0 == memcmp(mmgsdi_refresh_info_ptr[slot_index]->aid.aid.data_ptr,
                            msg_ptr->app_id_data.data_ptr,
                            msg_ptr->app_id_data.data_len)))
            {
              UIM_MSG_ERR_1("App is already being activated on slot 0x%x",
                            msg_ptr->request_header.slot_id);
              return MMGSDI_ERROR;
            }
          }
        }
      }
    }
  }

  /* If an UICC app is being activated, verify if the same app is
     already activated on another Session */
  if((msg_ptr->app_id_data.data_len > 0) &&
     (msg_ptr->app_id_data.data_len <= MMGSDI_MAX_AID_LEN))
  {
    mmgsdi_aid_type app_data;

    memset((void*)&app_data, 0x00, sizeof(mmgsdi_aid_type));

    /* Copy the app. data from the request */
    app_data.aid.data_len = msg_ptr->app_id_data.data_len;
    mmgsdi_memscpy((void*) app_data.aid.data_ptr,
                   sizeof(app_data.aid.data_ptr),
                   (const void*)msg_ptr->app_id_data.data_ptr,
                   msg_ptr->app_id_data.data_len);

    mmgsdi_status = mmgsdi_util_is_prov_app(&app_data,
                                            msg_ptr->request_header.slot_id,
                                            &is_app_active);

    if((mmgsdi_status == MMGSDI_SUCCESS) && (is_app_active == TRUE))
    {
      UIM_MSG_ERR_0("Cannot provision an app thats already activated on another session");
      return MMGSDI_ERROR;
    }
  }

  /* If the card is ICC card then use DEFAULT channel for the activation
     request. No need to open a new channel. */
  if (is_icc_card)
  {
    return mmgsdi_session_build_uim_open_channel_rsp(
             MMGSDI_DEFAULT_CHANNEL, /* channel id */
             msg_ptr->request_header,
             NULL,
             UIM_PASS,
             FALSE);
  }

  /* Start the activation procedures */
  return mmgsdi_session_uicc_open_channel_chk(msg_ptr);
}/* mmgsdi_session_activate_provisioning_start */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_SWITCH_PROVISIONING_START

   DESCRIPTION:
     This function will verify if the switch provisioning can be started, and
     queue the deactivation of the app currently activatied on the requested
     session and save the activate/switch request parameters for later use.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
static mmgsdi_return_enum_type mmgsdi_session_switch_provisioning_start(
  mmgsdi_session_act_or_switch_prov_req_type *msg_ptr
)
{
  mmgsdi_int_app_info_type          *app_info_ptr        = NULL;
  mmgsdi_channel_info_type          *channel_info_ptr    = NULL;
  mmgsdi_return_enum_type            mmgsdi_status       = MMGSDI_ERROR;
  uint8                              slot_index          = MMGSDI_SLOT_1_INDEX;

  MMGSDIUTIL_RETURN_IF_NULL(msg_ptr);

  mmgsdi_status = mmgsdi_util_get_prov_session_info(
                      msg_ptr->request_header.session_id,
                      NULL,
                      NULL,
                      &app_info_ptr);

  /* Not a prov. app */
  if((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
  {
    UIM_MSG_ERR_0("Cannot provision a non-prov Session");
    return MMGSDI_ERROR;
  }
  mmgsdi_status = mmgsdi_util_get_slot_index(msg_ptr->request_header.slot_id,
                                             &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }
  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    return MMGSDI_ERROR;
  }
  if(mmgsdi_refresh_info_ptr[slot_index] != NULL)
  {
    UIM_MSG_ERR_1("A REFRESH is in progress on slot 0x%x, cannot switch app",
                  msg_ptr->request_header.slot_id);
    return MMGSDI_SIM_BUSY;
  }

  mmgsdi_status = mmgsdi_util_get_session_and_channel_info(msg_ptr->request_header.session_id,
                      NULL,
                      &channel_info_ptr);
  if((mmgsdi_status != MMGSDI_SUCCESS) || (channel_info_ptr == NULL))
  {
    UIM_MSG_ERR_0("Could not get channel_info");
    return mmgsdi_status;
  }

  /* "is_dest_app" flag is passed as TRUE which means we need to wait for the
     callback for this de-activation (this is requested session which is
     currently bind to some other app) and from callback further proceed with
     activation. This will be called at the first place to free the requested
     session. No matter whether the requested app is free or not */
  mmgsdi_status = mmgsdi_session_deactivate_found_mismatch(msg_ptr,
                                                           msg_ptr->request_header.session_id,
                                                           TRUE);

  return mmgsdi_status;
}/* mmgsdi_session_switch_provisioning_start */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_PROC_ACT_OR_SWITCH_PROV

   DESCRIPTION:
     This function will perform parameter checks, generate session ID, save
     the data into the client reg table and proceed with the Manage Channel
     Open command to the card

   DEPENDENCIES:


   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                              but the service table indicates the card does not
                              have the support

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_session_proc_act_or_switch_prov(
  mmgsdi_session_act_or_switch_prov_req_type *msg_ptr
)
{
  mmgsdi_return_enum_type            mmgsdi_status       = MMGSDI_SUCCESS;
  uint32                             i                   = 0;
  mmgsdi_session_id_type             session_id          = 0;
  mmgsdi_session_info_type          *session_ptr         = NULL;
  mmgsdi_int_app_info_type          *app_info_ptr        = NULL;
  mmgsdi_channel_info_type          *channel_info_ptr    = NULL;
  mmgsdi_slot_data_type             *slot_data_ptr       = NULL;
  boolean                            is_icc_card         = FALSE;
  mmgsdi_session_id_status_enum_type session_status      = MMGSDI_SESSION_ID_UNASSIGNED;
  mmgsdi_int_app_info_type        ** app_pin_tbl_pptr    = NULL;
  mmgsdi_sw_status_type              status_word         = {0};
  uint8                              slot_index          = MMGSDI_SLOT_1_INDEX;
  mmgsdi_aid_type                    aid;
  mmgsdi_session_cnf_add_info_type   session_cnf_info;
  uint8                              prov_index          = MMGSDI_MAX_PROV_APPS;
#ifdef FEATURE_MODEM_CONFIG_REFRESH
  boolean                            nv_refresh_enabled  = FALSE;
  mcfg_refresh_index_type            mcfg_slot_index     = MCFG_REFRESH_INDEX_INVALID;
#endif /*FEATURE_MODEM_CONFIG_REFRESH*/
  mmgsdi_nv_context_type             sub_nv_context      = MMGSDI_NV_CONTEXT_INVALID;

  MMGSDIUTIL_RETURN_IF_NULL_2(msg_ptr,
                              msg_ptr->request_header.response_cb);

  memset(&aid,0x00, sizeof(mmgsdi_aid_type));
  memset(&session_cnf_info,0x00, sizeof(mmgsdi_session_cnf_add_info_type));
  /* -----------------------------------------------------------
  1) Check for valid session ID
  2) Check for application ID
  3) Obtain session index
  -------------------------------------------------------------- */
  session_id = msg_ptr->request_header.session_id;

  if((msg_ptr->app_id_data.data_len <= MMGSDI_MAX_AID_LEN) &&
     (msg_ptr->app_id_data.data_len >= 0))
  {
    aid.aid.data_len = msg_ptr->app_id_data.data_len;

    mmgsdi_memscpy((void*)aid.aid.data_ptr,
                   sizeof(aid.aid.data_ptr),
                   (void*)msg_ptr->app_id_data.data_ptr,
                   int32touint32(aid.aid.data_len));
  }
  else
  {
    UIM_MSG_ERR_0("Input app data is not valid");
    return MMGSDI_INCORRECT_PARAMS;
  }

  mmgsdi_status = mmgsdi_util_get_session_app_info(session_id,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   &session_ptr,
                                                   NULL);

  if(mmgsdi_status != MMGSDI_SUCCESS ||
     session_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  prov_index = mmgsdi_util_get_prov_index_from_session(session_ptr->session_type);
  if(prov_index == MMGSDI_MAX_PROV_APPS)
  {
    return MMGSDI_ERROR;
  }

#ifdef FEATURE_MODEM_CONFIG_REFRESH
  if(mmgsdi_util_get_mcfg_refresh_index(msg_ptr->request_header.slot_id, &mcfg_slot_index) != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  nv_refresh_enabled = mcfg_refresh_autoselect_enabled(mcfg_slot_index);
  UIM_MSG_MED_1("MCFG REFRESH status in nv - 0x%x",nv_refresh_enabled);

  /* MCFG NV refresh gets delayed if emergency call is in progress or device is
     in emergency call back mode. Send error response for the activation request.*/
  if(nv_refresh_enabled &&
     (mmgsdi_generic_data_ptr == NULL ||
      mmgsdi_generic_data_ptr->emergency_call_active[prov_index] ||
      mmgsdi_generic_data_ptr->ecbm_mode_active))
  {
    UIM_MSG_ERR_0("Reject Activate/Switch due to on going emergency call or ECBM");
    return MMGSDI_ERROR;
  }
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

  if(!mmgsdi_util_is_prov_session(session_ptr->session_type))
  {
    UIM_MSG_ERR_2("Cannot provision app on non-prov Session, id: 0x%x, type: 0x%x",
                  session_id, session_ptr->session_type);

    return MMGSDI_INCORRECT_PARAMS;
  }

  /* Determine the card type */
  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(msg_ptr->request_header.slot_id);
  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_is_card_icc(slot_data_ptr,&is_icc_card);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_get_slot_index(msg_ptr->request_header.slot_id,
                                             &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }
  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    return MMGSDI_ERROR;
  }

  /*Activate 1x session only when CDMALess NV is disbaled */
  if((mmgsdi_util_is_1x_prov_session(session_ptr->session_type)) &&
     (mmgsdi_util_get_efs_item_index(session_id, &sub_nv_context) == MMGSDI_SUCCESS) &&
     (mmgsdi_nv_get_external_feature_status(
        MMGSDI_EXT_FEAT_CDMA_LESS_DEVICE, sub_nv_context) == MMGSDI_FEATURE_ENABLED))
  {
    UIM_MSG_ERR_2("CDMALess NV is enabled for 1x subscription context 0x%x slot 0x%x; not activating",
                  sub_nv_context, msg_ptr->request_header.slot_id);
    return MMGSDI_NOT_SUPPORTED;
  }

  /* For ICC card map sesssion with GSM/CDMA DF. */
  if(is_icc_card)
  {
    app_pin_tbl_pptr = mmgsdi_util_get_app_pin_info_tbl_ptr(
                       msg_ptr->request_header.slot_id);
    if(app_pin_tbl_pptr == NULL)
    {
      return MMGSDI_ERROR;
    }
    /* Obtain the App type of the ICC slot which we are trying to provision to */
    for(i = 0; i < MMGSDI_MAX_EXT_APP_INFO; i++)
    {
      if(app_pin_tbl_pptr[i] == NULL)
      {
        continue;
      }
      if((((session_ptr->session_type == MMGSDI_1X_PROV_PRI_SESSION) ||
           (session_ptr->session_type == MMGSDI_1X_PROV_SEC_SESSION) ||
           (session_ptr->session_type == MMGSDI_1X_PROV_TER_SESSION)) &&
          (app_pin_tbl_pptr[i]->app_data.app_type == MMGSDI_APP_RUIM))
                              ||
         (((session_ptr->session_type == MMGSDI_GW_PROV_PRI_SESSION) ||
           (session_ptr->session_type == MMGSDI_GW_PROV_SEC_SESSION) ||
           (session_ptr->session_type == MMGSDI_GW_PROV_TER_SESSION)) &&
          (app_pin_tbl_pptr[i]->app_data.app_type == MMGSDI_APP_SIM)))
      {
        aid.app_type = app_pin_tbl_pptr[i]->app_data.app_type;
        break;
      }
    }

    if(i == MMGSDI_MAX_EXT_APP_INFO)
    {
      UIM_MSG_ERR_1(
        "No valid app.(SIM/RUIM) found for Session type: 0x%x, cannot do activate/switch operation",
        session_ptr->session_type);

      return MMGSDI_ERROR;
    }

#ifndef FEATURE_MMGSDI_3GPP2
    if(aid.app_type == MMGSDI_APP_RUIM)
    {
      UIM_MSG_ERR_1("App type 0x%x not supported", aid.app_type);
      return MMGSDI_ERROR;
    }
#endif /* FEATURE_MMGSDI_3GPP2 */
  }
  /* Check aid data for UICC card */
  else
  {
    app_pin_tbl_pptr = mmgsdi_util_get_app_pin_info_tbl_ptr(
                           msg_ptr->request_header.slot_id);
    if(app_pin_tbl_pptr == NULL)
    {
      return MMGSDI_ERROR;
    }
    for(i=0; i<MMGSDI_MAX_EXT_APP_INFO; i++)
    {
      if(app_pin_tbl_pptr[i] == NULL)
      {
        continue;
      }
      if(mmgsdi_util_cmp_aid(&app_pin_tbl_pptr[i]->app_data,&aid, FALSE) ==
                                                              MMGSDI_SUCCESS)
      {
        if( ((app_pin_tbl_pptr[i]->app_data.app_type == MMGSDI_APP_CSIM) &&
             ((session_ptr->session_type == MMGSDI_1X_PROV_PRI_SESSION) ||
              (session_ptr->session_type == MMGSDI_1X_PROV_SEC_SESSION) ||
              (session_ptr->session_type == MMGSDI_1X_PROV_TER_SESSION)))
                ||
            ((app_pin_tbl_pptr[i]->app_data.app_type == MMGSDI_APP_USIM) &&
             ((session_ptr->session_type == MMGSDI_GW_PROV_PRI_SESSION) ||
              (session_ptr->session_type == MMGSDI_GW_PROV_SEC_SESSION) ||
              (session_ptr->session_type == MMGSDI_GW_PROV_TER_SESSION)))
          )
        {
          aid.app_type = app_pin_tbl_pptr[i]->app_data.app_type;
          break;
        }
      }
    }
    if(i == MMGSDI_MAX_EXT_APP_INFO)
    {
      UIM_MSG_ERR_0("Invalid input app id, can not do switch or activate operation");
      return MMGSDI_ERROR;
    }
#ifndef FEATURE_MMGSDI_3GPP2
    if(aid.app_type == MMGSDI_APP_CSIM)
    {
      UIM_MSG_ERR_1("App type 0x%x not supported", aid.app_type);
      return MMGSDI_ERROR;
    }
#endif
  }

  session_status = mmgsdi_util_is_session_id_valid(session_id);
  UIM_MSG_MED_2("Session ID and session status 0x%x and 0x%x", session_id, session_status);
  /* Check session status */
  switch(session_status)
  {
    case MMGSDI_SESSION_ID_ASSIGNED_AND_ACTIVE:
      /* compare the aid  in request with aid for the activated app */
      mmgsdi_status = mmgsdi_util_get_session_app_info(
                        session_id,
                        NULL,
                        NULL,
                        &app_info_ptr,
                        &channel_info_ptr,
                        NULL,
                        NULL);

      if((mmgsdi_status != MMGSDI_SUCCESS) || (channel_info_ptr == NULL) ||
         (app_info_ptr == NULL))
      {
        return MMGSDI_ERROR;
      }

      mmgsdi_status =  mmgsdi_util_cmp_aid(&app_info_ptr->app_data,
                                           &aid,
                                           FALSE);

      /* If an attempt is made to activate an app which is
         already active on the current session, just return success */
      if((mmgsdi_status == MMGSDI_SUCCESS) &&
         (channel_info_ptr->slot_id == msg_ptr->request_header.slot_id))
      {
        /* Build Confirmation */
        mmgsdi_memscpy((void *)&session_cnf_info.app_info,
          sizeof(session_cnf_info.app_info),
          (void *)&app_info_ptr->app_data,
          sizeof(mmgsdi_aid_type));
        return mmgsdi_cnf_build_and_queue(mmgsdi_status,
                                          &msg_ptr->request_header,
                                          NULL,
                                          &session_cnf_info,
                                          FALSE, /* sw not required */
                                          status_word);
      }
      else
      {
        /* Current AID and requested AID dont match or the new app
           is being requested to be activated on a different slot  */
        if(mmgsdi_util_is_prov_session(session_ptr->session_type))
        {
          return mmgsdi_session_switch_provisioning_start(msg_ptr);
        }
        else
        {
          UIM_MSG_ERR_0("Can not do switch or activate operation on non-prov app");
          return MMGSDI_ERROR;
        }
      }
      /* since every conditional block above returns, no break necessary */
    case MMGSDI_SESSION_ID_ASSIGNED_NOT_ACTIVE:
      /* 1. Session is NOT mapped with an channel entry in
            mmgsdi_channel_info_ptr_table[] */
      return mmgsdi_session_pre_prov_activation_start(msg_ptr);

    case MMGSDI_SESSION_ID_UNASSIGNED:
    default:
      UIM_MSG_ERR_0("Invalid session id = 0x%x,session_id");
      break;
  }
  return MMGSDI_ERROR;
} /* mmgsdi_session_proc_act_or_switch_prov */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_SEND_STATUS_BEFORE_DEACTIVATE

   DESCRIPTION:
     This function will send the STATUS command before deactivating a UICC
     appclication

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                              but the service table indicates the card does not
                              have the support

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_session_send_status_before_deactivate(
  mmgsdi_session_id_type    session_id,
  mmgsdi_slot_id_enum_type  slot_id,
  const mmgsdi_aid_type   * app_data_ptr
)
{
  mmgsdi_status_req_type       status_req;
  mmgsdi_status_cnf_type     * cnf_ptr       = NULL;
  mmgsdi_return_enum_type      mmgsdi_status = MMGSDI_ERROR;

  memset(&status_req, 0, sizeof(status_req));

  status_req.request_header.request_type      = MMGSDI_CARD_STATUS_REQ;
  status_req.request_header.orig_request_type = MMGSDI_CARD_STATUS_REQ;
  status_req.request_header.session_id        = session_id;
  status_req.request_header.slot_id           = slot_id;
  status_req.me_app_status                    = MMGSDI_STATUS_APP_TERMINATED;
  status_req.ret_data_struct                  = MMGSDI_STATUS_NO_DATA;

  mmgsdi_status = mmgsdi_uim_uicc_status(&status_req, TRUE, &cnf_ptr, app_data_ptr);

  mmgsdi_util_free_response_ptr((mmgsdi_cnf_type *) cnf_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cnf_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_MED_0("Unable to send STATUS command before app termination");
  }

  return mmgsdi_status;
} /* mmgsdi_session_send_status_before_deactivate */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_DEACTIVATE_START

   DESCRIPTION:
     This function will perform parameter checks and proceed with the building
     of SELECT AID with deactivate request

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                              but the service table indicates the card does not
                              have the support

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_session_deactivate_start(
  mmgsdi_session_deact_req_type *msg_ptr
)
{
  mmgsdi_int_app_info_type         *app_info_ptr         = NULL;
  mmgsdi_return_enum_type           mmgsdi_status        = MMGSDI_ERROR;
  mmgsdi_get_file_attr_cnf_type    *cnf_ptr              = NULL;
  mmgsdi_get_file_attr_req_type     get_file_attr_msg    = {{0}};
  int32                             index                = 0;
  boolean                           skip_app_termination = FALSE;
  mmgsdi_slot_id_enum_type          slot_id              = MMGSDI_SLOT_1;

  MMGSDIUTIL_RETURN_IF_NULL(msg_ptr);

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                    msg_ptr->request_header.session_id,
                    &slot_id, NULL, &app_info_ptr, NULL, NULL, NULL);

  if((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
  {
    UIM_MSG_ERR_2("Invalid app for session id=0x%x, app_info_ptr=0x%x",
                  msg_ptr->request_header.session_id, app_info_ptr);
    return mmgsdi_status;
  }

  /* In case of USIM or CSIM apps determine if there is only one such app of
     that type on the slot so that it can be marked as deactivated without
     sending app termination to the card. */
  if ((msg_ptr->request_header.orig_request_type == MMGSDI_SESSION_DEACTIVATE_REQ) &&
      (mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_SKIP_PROV_APP_TERMINATION, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) ==
        MMGSDI_FEATURE_ENABLED))
  {
    if (app_info_ptr->app_data.app_type == MMGSDI_APP_USIM &&
        (mmgsdi_util_get_usim_apps(slot_id) == 1) )
    {
      skip_app_termination = TRUE;
    }
    else if (app_info_ptr->app_data.app_type == MMGSDI_APP_CSIM &&
             (mmgsdi_util_get_csim_apps(slot_id) == 1) )
    {
      skip_app_termination = TRUE;
    }
    UIM_MSG_MED_2("app_type: 0x%x, skip_app_termination: 0x%x",
                  app_info_ptr->app_data.app_type, skip_app_termination);
  }

  if(((app_info_ptr->app_data.app_type == MMGSDI_APP_USIM) ||
      (app_info_ptr->app_data.app_type == MMGSDI_APP_CSIM)) &&
     !skip_app_termination)
  {
    /* Send a STATUS command (synchronously) to indicate to the card that
       the app is going to be terminated in the modem. */
    mmgsdi_status = mmgsdi_session_send_status_before_deactivate(
                      msg_ptr->request_header.session_id,
                      slot_id,
                      NULL);

    get_file_attr_msg.access.access_method = MMGSDI_BY_APP_ID_ACCESS;
    get_file_attr_msg.access.file.app_id.data_len = app_info_ptr->app_data.aid.data_len;

    if((app_info_ptr->app_data.aid.data_len <= MMGSDI_MAX_AID_LEN) &&
       (app_info_ptr->app_data.aid.data_len > 0))
    {
      mmgsdi_memscpy((void *)get_file_attr_msg.access.file.app_id.data_ptr,
                     sizeof(get_file_attr_msg.access.file.app_id.data_ptr),
                     (void *)app_info_ptr->app_data.aid.data_ptr,
                     int32touint32(app_info_ptr->app_data.aid.data_len));
    }

    UIM_MSG_HIGH_1("Sending down UICC SELECT AID command to MMGSDI UIM: aid len 0x%x",
                   app_info_ptr->app_data.aid.data_len);

    /* This is a deactivate request */
    get_file_attr_msg.activate_aid = FALSE;

    mmgsdi_memscpy((void *)&get_file_attr_msg.request_header,
                   sizeof(get_file_attr_msg.request_header),
                   (void *)&msg_ptr->request_header,
                   sizeof(mmgsdi_request_header_type));

    /* extra_param_req is passed in as FALSE so that
       extra parameters are not allocated for this request.
       When the UIM response is handled in mmgsdi_session_rsp_deactivate(),
       the absence/presence of extra params is used to dedeuce that its an
       AID deactivate/MANAGE CHANNEL confirmation */
    return mmgsdi_uim_uicc_select (&get_file_attr_msg,
                                   FALSE,
                                   &cnf_ptr,
                                   FALSE);
  }
  else if (((app_info_ptr->app_data.app_type == MMGSDI_APP_CSIM) ||
            (app_info_ptr->app_data.app_type == MMGSDI_APP_USIM)) &&
           skip_app_termination )
  {
    /* Mark channel state appropriately if we choose not to terminate the app
       as part of session deactivation. */
    mmgsdi_channel_info_type * channel_info_ptr = NULL;

    mmgsdi_status = mmgsdi_util_get_session_and_channel_info(
                      msg_ptr->request_header.session_id,
                      NULL,
                      &channel_info_ptr);
    if (mmgsdi_status == MMGSDI_SUCCESS && channel_info_ptr != NULL)
    {
      channel_info_ptr->app_termination_skipped = TRUE;
    }
  }

  /* For ICC cards and UICC cards where app termination is not performed,
     queue a dummy UIM response */

  /*---------------------------------------------------------------------------
    Store Information into the client req info table
    1) Get the index
    2) Populate client request table
    ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                    index,
                    &msg_ptr->request_header,
                    NULL,
                    NULL);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }

  /* send out the response */
  mmgsdi_status =  mmgsdi_util_queue_mmgsdi_uim_report_rsp(
                    index, msg_ptr->request_header.request_type,
                    MMGSDI_SUCCESS);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("Could not queue dummy UIM response");
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }
  return MMGSDI_SUCCESS;
}/* mmgsdi_session_deactivate_start */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_PROC_DEACTIVATE

   DESCRIPTION:
     This function will verfify if app deactivate can be started.
     For prov. apps, a APP_RESET_AUTO REFRESH is started to  vote
     with clients and figure out if the app can be dactivated

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:
     None
-------------------------------------------------------------------------------*/
static mmgsdi_return_enum_type mmgsdi_session_proc_deactivate(
  mmgsdi_session_deact_req_type *msg_ptr
)
{
  mmgsdi_int_app_info_type         *app_info_ptr        = NULL;
  mmgsdi_return_enum_type           mmgsdi_status       = MMGSDI_ERROR;
  int32                             index               = 0;
  uint8                             slot_index          = MMGSDI_SLOT_1_INDEX;

  UIM_MSG_HIGH_0("MMGSDI_SESSION_PROC_DEACTIVATE");

  MMGSDIUTIL_RETURN_IF_NULL(msg_ptr);

  /* Need to verify if clients are OK to deactivate this app, check this only if the request
     is for deactivation */
  if(msg_ptr->request_header.orig_request_type != MMGSDI_SESSION_DEACTIVATE_REQ)
  {
    return mmgsdi_session_deactivate_start(msg_ptr);
  }
  else
  {
    mmgsdi_status = mmgsdi_util_get_prov_session_info(
                      msg_ptr->request_header.session_id,
                      NULL,
                      NULL,
                      &app_info_ptr);

    /* Not a prov. app */
    if((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
    {
      return mmgsdi_session_deactivate_start(msg_ptr);
    }

    mmgsdi_status = mmgsdi_util_get_slot_index(msg_ptr->request_header.slot_id,
                                               &slot_index);
    if((mmgsdi_status != MMGSDI_SUCCESS) ||
       ((slot_index != MMGSDI_SLOT_1_INDEX) &&
        (slot_index != MMGSDI_SLOT_2_INDEX) &&
        (slot_index != MMGSDI_SLOT_3_INDEX)))
    {
      return MMGSDI_ERROR;
    }

    if((mmgsdi_refresh_info_ptr[slot_index] != NULL) &&
       (mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_APP_RESET_AUTO) &&
       (MMGSDI_SUCCESS == mmgsdi_util_cmp_aid(&(mmgsdi_refresh_info_ptr[slot_index]->aid),
                                              &(app_info_ptr->app_data),
                                              FALSE)))
    {
      UIM_MSG_ERR_0("App deactivation is already in progress, rejecting this request");
      return MMGSDI_ERROR;
    }

    /*if we are here because of internal deactivation req, start deactivation*/
    if(msg_ptr->internal_deact_req)
    {
      /* If this deactivation is because of mismatch then first check whether
         the request is on correct correct task or not because it is possible
         that by this time the session mappings got changed. If wrongly
         queued then reject the request otherwise proceed with deact */
      if (msg_ptr->is_reason_mismatch)
      {
        mmgsdi_task_enum_type    current_task   = MMGSDI_TASK_MAX;
        mmgsdi_task_enum_type    deact_req_task = MMGSDI_TASK_MAX;
        mmgsdi_slot_id_enum_type slot_id        = MMGSDI_MAX_SLOT_ID_ENUM;

        mmgsdi_status = mmgsdi_util_get_session_app_info(msg_ptr->request_header.session_id,
                                                         &slot_id,
                                                         NULL, NULL, NULL, NULL, NULL);

        if (mmgsdi_status != MMGSDI_SUCCESS)
        {
          return MMGSDI_ERROR;
        }

        deact_req_task = mmgsdi_util_get_task_from_slot(slot_id);
        current_task = mmgsdi_util_get_current_task();

        if (deact_req_task != current_task)
        {
          UIM_MSG_ERR_3("current_task: 0x%x not matching with request_task: 0x%x for session_id: 0x%x",
                        current_task, deact_req_task,
                        msg_ptr->request_header.session_id);
          return MMGSDI_ERROR;
        }

        /* Update the session close cause with MISMATCH FOUND */
        mmgsdi_util_update_session_closed_cause(msg_ptr->request_header.session_id,
                                                MMGSDI_SESSION_CLOSED_CAUSE_MISMATCH_FOUND);
      }
      return mmgsdi_session_deactivate_start(msg_ptr);
    }

    /*Fill the client Table*/
    mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }

    mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                      index,
                      &msg_ptr->request_header,
                      NULL,
                      NULL);

    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      mmgsdi_util_free_client_request_table_index(index);
      return mmgsdi_status;
    }

    MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT
    app_info_ptr->deactivation_in_progress = TRUE;
    MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT

   /* Refresh with mode as RESET_AUTO is going to start. Fail on-going refresh
      if any on the same slot and same app. This is done to make sure that the
      deactivation process would be quick.*/
    if((mmgsdi_refresh_info_ptr[slot_index] != NULL) &&
       (mmgsdi_refresh_info_ptr[slot_index]->request_header.slot_id == msg_ptr->request_header.slot_id) &&
       (MMGSDI_SUCCESS == mmgsdi_util_cmp_aid(&(mmgsdi_refresh_info_ptr[slot_index]->aid),
                                              &(app_info_ptr->app_data),
                                              FALSE)))
    {
      mmgsdi_card_refresh_extra_info_type  extra_info;

      UIM_MSG_HIGH_0("Stop ongoing REFRESH to allow APP deactivation");

      memset(&extra_info, 0x00, sizeof(mmgsdi_card_refresh_extra_info_type));

      mmgsdi_refresh_info_ptr[slot_index]->refresh_status = MMGSDI_CAN_NOT_REFRESH;

      /* Send END_FAILED to the clients */
      extra_info.mode     = mmgsdi_refresh_info_ptr[slot_index]->mode;
      extra_info.send_evt = TRUE;
      extra_info.stage    = MMGSDI_REFRESH_STAGE_END_FAILED;

      (void)mmgsdi_refresh_send_evt(&extra_info,
                                    &mmgsdi_refresh_info_ptr[slot_index]->request_header);

      (void)mmgsdi_refresh_rsp_send_cnf_to_gstk(
              &mmgsdi_refresh_info_ptr[slot_index]->request_header);

      mmgsdi_refresh_retry_clear_table(slot_index);
    }

    /* Start APP RESET AUTO to determine if the clients are OK
       with the app deactivate */
    mmgsdi_session_start_refresh_auto(&msg_ptr->request_header,
                                      &app_info_ptr->app_data,
                                      msg_ptr->request_header.slot_id,
                                      index);
  }
  return MMGSDI_SUCCESS;
}/* mmgsdi_session_proc_deactivate */


/*===========================================================================
FUNCTION:      MMGSDI_SESSION_BUILD_STATUS_TERMINATE

DESCRIPTION:
  This function is used to build a status command for a particular
  application termination

DEPENDENCIES:

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  If status command is executed succesfully, it builds up a command to
  terminate the app also.
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_session_build_status_terminate (
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type card_slot,
  mmgsdi_cmd_enum_type     orig_req_cmd,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
)
{
  int32                          client_index       = MMGSDI_MAX_CLIENT_INFO;
  int32                          session_index      = MMGSDI_MAX_SESSION_INFO;
  mmgsdi_return_enum_type        mmgsdi_status      = MMGSDI_SUCCESS;
  mmgsdi_task_cmd_type          *task_cmd_ptr       = NULL;
  mmgsdi_status_req_type        *status_msg_ptr     = NULL;
  int32                          total_mem_len      = 0;
  uint32                         task_cmd_len       = 0;
  mmgsdi_cmd_enum_type           curr_req_cmd       = MMGSDI_CARD_STATUS_REQ;
  mmgsdi_channel_id_type         channel_id         = 0;

  UIM_MSG_HIGH_2("MMGSDI_SESSION_BUILD_STATUS, session id 0x%x, slot 0x%x",
                 session_id, card_slot);

  /* get the client id index from client_id*/
  mmgsdi_status = mmgsdi_util_get_client_and_session_index(
                    session_id, &client_index, &session_index);

  if(mmgsdi_status != MMGSDI_SUCCESS ||
     (client_index < 0) || (client_index >= MMGSDI_MAX_CLIENT_INFO) ||                        \
     (session_index >= MMGSDI_MAX_SESSION_INFO) || (session_index < 0))
  {
    UIM_MSG_ERR_2("Invalid client index:0x%x, session index:0x%x",
                   client_index, session_index);
    return mmgsdi_status;
  }

  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  task_cmd_len  = int32touint32(total_mem_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - Align pointers of the various structures members
  ---------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.status_req,
                         curr_req_cmd);
  status_msg_ptr = &task_cmd_ptr->cmd.cmd.status_req;

  /* Get the channel_id for the STATUS command, but for card RESET req,
     channel_id gets defaulted to channel 0 */
  if(orig_req_cmd != MMGSDI_CARD_RESET_REQ)
  {
    (void) mmgsdi_util_get_session_app_info(session_id,
                                            NULL,
                                            &channel_id,
                                            NULL,
                                            NULL,
                                            NULL,
                                            NULL);
  }

  /*---------------------------------------------------------------------------
    POPULATE THE REQUEST MESSAGE PAYLOAD
    1. client_id       - Not Used
    2. session_id      - Session Id Obtained from cmd request
    3. request_type    - Request Command Type
    4. request_len     - Length of message + header
    5. payload_len     - Length of the message payload
    6. slot_id         - Slot that the request is to be performed on
    7. client_data     - Pointer to Client Data
    8. response_cb     - Response Callback Pointer
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                       = curr_req_cmd;
  status_msg_ptr->request_header.client_id         = 0;
  status_msg_ptr->request_header.session_id        = session_id;
  status_msg_ptr->request_header.channel_id        = channel_id;
  status_msg_ptr->request_header.request_type      = curr_req_cmd;
  status_msg_ptr->request_header.orig_request_type = orig_req_cmd;
  status_msg_ptr->request_header.slot_id           = card_slot;
  status_msg_ptr->request_header.client_data       = client_ref;
  status_msg_ptr->request_header.response_cb       = response_cb_ptr;
  status_msg_ptr->request_header.request_len       = total_mem_len;
  status_msg_ptr->request_header.payload_len       = status_msg_ptr->request_header.request_len -
                                                       uint32toint32(sizeof(mmgsdi_request_header_type));

  /*---------------------------------------------------------------------------
    POPULATE THE REQUEST MESSAGE PAYLOAD
    1.  me_app_status       -  MMGSDI_STATUS_APP_TERMINATED
                               ( indicate app termination )
    2.  ret_data_struct     -  MMGSDI_STATUS_NO_DATA
                               ( no need to return any data except status words)
  ---------------------------------------------------------------------------*/
  status_msg_ptr->me_app_status   = MMGSDI_STATUS_APP_TERMINATED;
  status_msg_ptr->ret_data_struct = MMGSDI_STATUS_NO_DATA;

  /* PLACE ON GSDI QUEUE */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(task_cmd_ptr);

  }
  return mmgsdi_status;
} /* mmgsdi_session_build_status_terminate */


/*===========================================================================
FUNCTION:      MMGSDI_SESSION_BUILD_DEACTIVATE

DESCRIPTION:
  This function is used to build a deactivate command for a particular
  application

DEPENDENCIES:
  The client must have a valid session open.
  Achived by calling mmgsdi_session_management_open.

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  Will result in your callback getting called everytime MMGSDI Needs to notify
  any client of any events.
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_session_build_deactivate (
  mmgsdi_client_id_type    client_id,
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type card_slot,
  mmgsdi_cmd_enum_type     orig_req_cmd,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
)
{
  mmgsdi_return_enum_type            mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_static_data_type           *aid_ptr       = NULL;
  mmgsdi_int_app_info_type          *app_info_ptr  = NULL;

  UIM_MSG_HIGH_2("MMGSDI_SESSION_MANAGEMENT_DEACTIVATE, client 0x%x, slot 0x%x",
                 client_id, card_slot);

  mmgsdi_status = mmgsdi_util_get_session_app_info(session_id,
                                                   NULL,
                                                   NULL,
                                                   &app_info_ptr,
                                                   NULL,
                                                   NULL,
                                                   NULL);

  if (mmgsdi_status != MMGSDI_SUCCESS || app_info_ptr == NULL)
  {
    return mmgsdi_status;
  }

  aid_ptr = &app_info_ptr->app_data.aid;

  return mmgsdi_session_build_select_aid (client_id,
                                          session_id,
                                          card_slot,
                                          orig_req_cmd,
                                          MMGSDI_SESSION_DEACTIVATE_REQ,
                                          *aid_ptr,
                                          FALSE,
                                          MMGSDI_SELECT_RSP_FCP,
                                          response_cb_ptr,
                                          client_ref);
} /* mmgsdi_session_build_deactivate */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_MAIN

   DESCRIPTION:
     This function will provide open session using the ICC protocol

   DEPENDENCIES:

   LIMITATIONS:

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                              but the service table indicates the card does not
                              have the support

   SIDE EFFECTS:

-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_session_main (
  mmgsdi_task_cmd_data_type      * cmd_ptr
)
{
  mmgsdi_return_enum_type           mmgsdi_status        = MMGSDI_SUCCESS;
  mmgsdi_cmd_enum_type              cmd                  = MMGSDI_MAX_CMD_ENUM;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    cmd_ptr                                - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(cmd_ptr);

  cmd = cmd_ptr->cmd_enum;

  switch (cmd)
  {
    case MMGSDI_SESSION_OPEN_REQ:
    case MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_REQ:
      mmgsdi_status = mmgsdi_session_proc_open(&cmd_ptr->cmd.session_open_req);
      break;

    case MMGSDI_SESSION_OPEN_EXT_REQ:
      mmgsdi_status = mmgsdi_session_proc_open_ext(&cmd_ptr->cmd.session_open_ext_req);
      break;

    case MMGSDI_SESSION_OPEN_WITH_MF_REQ:
      mmgsdi_status = mmgsdi_session_proc_open_with_mf(
                        &cmd_ptr->cmd.session_open_with_mf_req);
      break;

    case MMGSDI_SESSION_CLOSE_REQ:
      mmgsdi_status = mmgsdi_session_proc_close(&cmd_ptr->cmd.session_close_req);
      break;

    case MMGSDI_SESSION_SELECT_AID_REQ:
      mmgsdi_status = mmgsdi_session_select_aid(&cmd_ptr->cmd.get_file_attr_req);
      break;

    case MMGSDI_SESSION_DEACTIVATE_REQ:
      if (cmd_ptr->cmd.session_deact_req.request_header.orig_request_type ==
           MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ)
      {
        mmgsdi_status =
          mmgsdi_session_select_aid(&cmd_ptr->cmd.get_file_attr_req);
      }
      else
      {
        mmgsdi_status =
          mmgsdi_session_proc_deactivate(&cmd_ptr->cmd.session_deact_req);
      }
      break;

    case MMGSDI_ISIM_AUTH_REQ:
      mmgsdi_status = mmgsdi_uim_uicc_isim_auth(&cmd_ptr->cmd.auth_req);
      break;

    case MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ:
      mmgsdi_status = mmgsdi_session_proc_act_or_switch_prov(
                        &cmd_ptr->cmd.session_act_or_switch_prov_req);
      break;

    default:
      UIM_MSG_ERR_1("Invalid command enum 0x%x",cmd);
      mmgsdi_status = MMGSDI_ERROR;
      break;
  }

  if (mmgsdi_status != MMGSDI_SUCCESS )
  {
    UIM_MSG_HIGH_2("ERROR cmd 0x%x failed with status 0x%x", cmd, mmgsdi_status);
  }
  return mmgsdi_status;
} /* mmgsdi_session_main */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_MAIN_RESPONSE

   DESCRIPTION:
     This function will handle the session management response processing.

   DEPENDENCIES:


   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                              but the service table indicates the card does not
                              have the support

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_session_main_response (
  mmgsdi_cmd_enum_type               cmd_type,
  const mmgsdi_uim_report_rsp_type * rsp_ptr
)
{
  mmgsdi_return_enum_type           mmgsdi_status        = MMGSDI_SUCCESS;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    rsp_ptr                                - Cannot be NULL
   ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(rsp_ptr);

  switch (cmd_type)
  {
    case MMGSDI_SESSION_OPEN_REQ:
    case MMGSDI_SESSION_OPEN_EXT_REQ:
    case MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_REQ:
    case MMGSDI_SESSION_OPEN_WITH_MF_REQ:
      mmgsdi_status = mmgsdi_session_rsp_open(rsp_ptr);
      break;

    case MMGSDI_SESSION_SELECT_AID_REQ:
      mmgsdi_status = mmgsdi_session_rsp_select_aid(rsp_ptr);
      break;

    case MMGSDI_SESSION_DEACTIVATE_REQ:
      mmgsdi_status = mmgsdi_session_rsp_deactivate(rsp_ptr);
      break;

    case MMGSDI_SESSION_CLOSE_REQ:
      mmgsdi_status = mmgsdi_session_rsp_close(rsp_ptr);
      break;

    case MMGSDI_ISIM_AUTH_REQ:
      mmgsdi_status = mmgsdi_session_rsp_isim_auth(rsp_ptr);
      break;

    case MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ:
      mmgsdi_status = mmgsdi_session_rsp_act_or_switch_prov(
                        rsp_ptr);
      break;

    default:
      mmgsdi_status = MMGSDI_ERROR;
      break;
  }

  return mmgsdi_status;
} /* mmgsdi_session_main_response */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_ADD_AID_TO_APP_INFO_TABLE

   DESCRIPTION:
     This function will add the AID to the global APP info table

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                              but the service table indicates the card does not
                              have the support

   SIDE EFFECTS:
     None.

   CRITICAL SECTIONS:
     The function uses mmgsdi_client_app_data_crit_sect_ptr to protect accesses
     to slot data ptr.
-------------------------------------------------------------------------------*/
static mmgsdi_return_enum_type mmgsdi_session_add_aid_to_app_info_table(
  mmgsdi_slot_id_enum_type                     slot_id,
  const mmgsdi_aid_type                       *aid_ptr,
  uint8                                        curr_aid_rec)
{
  mmgsdi_slot_data_type   *slot_data_ptr = NULL;
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_SUCCESS;
  uint8                    slot_index    = MMGSDI_SLOT_1_INDEX;

  UIM_MSG_HIGH_0("mmgsdi_session_add_aid_to_app_info_table");

  MMGSDIUTIL_RETURN_IF_NULL(aid_ptr);

  /* Check curr_aid_rec is in range */
  if (curr_aid_rec >= MMGSDI_MAX_EXT_APP_INFO)
  {
    UIM_MSG_ERR_1("curr_aid_rec >= MMGSDI_MAX_EXT_APP_INFO, 0x%x",
                  curr_aid_rec);
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_get_slot_index(slot_id, &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);
  /* Check for valid slot_data_ptr */
  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  if(mmgsdi_app_pin_table_ptr[slot_index][curr_aid_rec] != NULL)
  {
    MMGSDI_MSG_ERROR_2("App table is not NULL, slot index 0x%x, app index 0x%x",
                      slot_index, curr_aid_rec);
    return MMGSDI_ERROR;
  }

  /* Allocating memory for the application*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
    mmgsdi_app_pin_table_ptr[slot_index][curr_aid_rec],
    sizeof(mmgsdi_int_app_info_type));

  if(mmgsdi_app_pin_table_ptr[slot_index][curr_aid_rec] == NULL)
  {
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  /* Copy the AID into the global data structure which holds the AID data */
  mmgsdi_memscpy((void*)&slot_data_ptr->app_info_pptr[curr_aid_rec]->app_data.aid,
    sizeof(slot_data_ptr->app_info_pptr[curr_aid_rec]->app_data.aid),
    (void*)&aid_ptr->aid,
    sizeof(mmgsdi_static_data_type));

  slot_data_ptr->app_info_pptr[curr_aid_rec]->app_selected = FALSE;
  slot_data_ptr->app_info_pptr[curr_aid_rec]->dir_index = 0;

  /* Copy the label into the global data structure  */
  mmgsdi_memscpy((void*)&slot_data_ptr->app_info_pptr[curr_aid_rec]->app_data.label,
                 sizeof(slot_data_ptr->app_info_pptr[curr_aid_rec]->app_data.label),
                 (void*)&aid_ptr->label,
                 sizeof(mmgsdi_static_data_type));

  /* Store the app type */
  slot_data_ptr->app_info_pptr[curr_aid_rec]->app_data.app_type =
    mmgsdi_util_determine_app_type_from_aid(&aid_ptr->aid);

  /* Set the app state to indicate a valid app is present */
  slot_data_ptr->app_info_pptr[curr_aid_rec]->app_state =
    MMGSDI_APP_STATE_DETECTED;

  if((slot_data_ptr->app_info_pptr[curr_aid_rec]->app_data.app_type == MMGSDI_APP_USIM) ||
     (slot_data_ptr->app_info_pptr[curr_aid_rec]->app_data.app_type == MMGSDI_APP_CSIM))
  {
    slot_data_ptr->app_info_pptr[curr_aid_rec]->proprietary_app   = FALSE;
  }
  else /* The App is of an unknown type ergo a proprietary app */
  {
    slot_data_ptr->app_info_pptr[curr_aid_rec]->proprietary_app   = TRUE;
    slot_data_ptr->app_info_pptr[curr_aid_rec]->app_not_in_efdir  = TRUE;
  }

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;
  return MMGSDI_SUCCESS;
} /* mmgsdi_session_add_aid_to_app_info_table */


/*===========================================================================
   FUNCTION:      MMGSDI_SESSION_ADD_AID_IF_NOT_PRESENT

   DESCRIPTION:
     This function will add the input AID to the session app info table
     if it is not present already.

     If the AID was added to the app info table, app_added_to_table will be
     set to TRUE.  Otherwise, it will be set to FALSE.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The AID was or already present in the AID.
     MMGSDI_ERROR:            The attempt to add AID was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:
     None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_session_add_aid_if_not_present(
  const mmgsdi_aid_type    * aid_ptr,
  mmgsdi_session_info_type * session_info_ptr,
  boolean                  * app_added_to_table_ptr
)
{
  mmgsdi_return_enum_type               mmgsdi_status           = MMGSDI_SUCCESS;
  mmgsdi_int_app_info_type           ** app_info_tbl_pptr       = NULL;
  int8                                  i                       = 0;
  int8                                  lowest_app_index        = -1;
  uint8                                 new_app_index           = 0;

  MMGSDIUTIL_RETURN_IF_NULL_2(app_added_to_table_ptr,
                              aid_ptr);
  *app_added_to_table_ptr = FALSE;

  /* Check for valid session_info_ptr */
  if((session_info_ptr == NULL) ||
     (session_info_ptr->channel_info_index >= MMGSDI_MAX_CHANNEL_INFO) ||
     (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index] == NULL))
  {
    UIM_MSG_ERR_0("Invalid Session info");
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* Find out if the AID selected is in the app table.  If not add to the table */
  app_info_tbl_pptr = mmgsdi_util_get_app_pin_info_tbl_ptr(
                        mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->slot_id);

  /* Check for valid app_info_tbl_ptr */
  if(app_info_tbl_pptr == NULL)
  {
    UIM_MSG_ERR_0("app_info_tbl_ptr is NULL");
    return MMGSDI_ERROR;
  }

  /* Check to see if the AID was in the EF-DIR file.  If it wasn't it means the
     AID not in EF-DIR was just selected.
     In that case the AID needs to be added to the app table. */
  for(i = 0; i < MMGSDI_MAX_EXT_APP_INFO; i++)
  {
    if(app_info_tbl_pptr[i] == NULL)
    {
      /* Check if the AID entry in the app table is available and keep track
         of app count */
      if(lowest_app_index == -1)
      {
        lowest_app_index = i;
      }
      continue;
    }
    if(MMGSDI_SUCCESS == mmgsdi_util_cmp_aid(&app_info_tbl_pptr[i]->app_data,
                                             aid_ptr,
                                             FALSE))
    {
      UIM_MSG_HIGH_1("Found AID in AID table %i, no need to add", i);
      /* If the app was found in the database, no need to add, just return.
         *app_added_to_table_ptr default value is FALSE, so caller will know the
         AID was not added to the app info table. */
      return MMGSDI_SUCCESS;
    }
  } /* for i < MMGSDI_MAX_EXT_APP_INFO */

  /* Proceed to add AID to app info table since it was not found in the table.
     But first check if there is space left in the app info table before adding */
  if(lowest_app_index == -1)
  {
    UIM_MSG_HIGH_0("App info table full, cannot open select AID");
    mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr = NULL;
    return MMGSDI_ERROR;
  }

  new_app_index = int8touint8(lowest_app_index);

  /* Add the application to App info table */
  mmgsdi_status = mmgsdi_session_add_aid_to_app_info_table(
                    mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->slot_id,
                    aid_ptr,
                    new_app_index);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("Unable to add the AID to the app info table");
    return MMGSDI_ERROR;
  }

  /* AID was successfully added, so set *app_added_to_table_ptr to TRUE. */
  *app_added_to_table_ptr = TRUE;
  return MMGSDI_SUCCESS;

} /* mmgsdi_session_add_aid_if_not_present */


/*===========================================================================
  FUNCTION MMGSDI_SESSION_CREATE_AND_QUEUE_INTERNAL_DEACTIVATE_REQ

DESCRIPTION
  Function to create MMGSDI_SESSION_DEACTIVATE_REQ cmd internally with and queue it

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_session_create_and_queue_internal_deactivate_req(
  mmgsdi_session_id_type session_id
)
{
  mmgsdi_task_cmd_type                *task_cmd_ptr    = NULL;
  mmgsdi_session_deact_req_type       *msg_ptr         = NULL;
  mmgsdi_return_enum_type              mmgsdi_status   = MMGSDI_ERROR;
  int32                                total_mem_len   = 0;
  uint32                               task_cmd_len    = 0;
  mmgsdi_session_id_status_enum_type   session_status  = MMGSDI_SESSION_ID_UNASSIGNED;
  /*---------------------------------------------------------------------------
   Input Parameters Check
   1) Session ID
   2) Response CB function Check
  ---------------------------------------------------------------------------*/
  session_status = mmgsdi_util_is_session_id_valid(session_id);

  if ((session_status != MMGSDI_SESSION_ID_ASSIGNED_AND_ACTIVE) &&
      (session_status != MMGSDI_SESSION_ID_ASSIGNED_NOT_ACTIVE))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  total_mem_len += uint32toint32(sizeof(mmgsdi_task_cmd_type));

  task_cmd_len = int32touint32(total_mem_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  msg_ptr = &task_cmd_ptr->cmd.cmd.session_deact_req;

  /*---------------------------------------------------------------------------
   Populating mmgsdi_session_deact_req_type header
     1) Session ID
     2) Request TYPE
     3) Response CallBack
  ---------------------------------------------------------------------------*/

  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_SESSION_DEACTIVATE_REQ;
  msg_ptr->internal_deact_req               = TRUE;
  msg_ptr->request_header.session_id        = session_id;
  msg_ptr->request_header.request_type      = MMGSDI_SESSION_DEACTIVATE_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_SESSION_DEACTIVATE_REQ;
  msg_ptr->request_header.client_data       = 0;
  msg_ptr->request_header.response_cb       = mmgsdi_util_internal_cb;

  /*---------------------------------------------------------------------------
   Calculate the payload len and request len
  ---------------------------------------------------------------------------*/
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len =
    total_mem_len - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  UIM_MSG_HIGH_1("Queue of internal mmgsdi_session_deact_req_type cmd, status: 0x%x",
                 mmgsdi_status);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put
       onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  return mmgsdi_status;
}/* mmgsdi_session_create_and_queue_internal_deactivate_req */


/*===========================================================================
  FUNCTION MMGSDI_SESSION_DEACTIVATE_MISMATCH_SESSIONS

DESCRIPTION
  This function will check whether the destination app is free or not.
  If no, then trigger de-activation for destination app. Also, this
  function will check if there is any wrong-mappings with respect to
  current req. If yes, then deactivate all those wrong mapped sessions
  also.

  halt_activation_ptr: This is an output parameter for knowing whether
  we need to return from caller or proceed with our activation process.
  In case there is some ERROR or destination App is not free we need to
  return and the activation will be triggered again from callback fn.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_session_deactivate_mismatch_sessions(
  const mmgsdi_session_act_or_switch_prov_req_type  *msg_ptr,
  boolean                                           *mismatch_or_error_detected_ptr)
{
  mmgsdi_session_type_enum_type  request_session_type       = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_session_type_enum_type  pair_session_type          = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_session_type_enum_type  dest_app_session_type      = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_session_id_type         pair_session_id            = 0;
  mmgsdi_session_id_type         dest_app_session_id        = 0;
  mmgsdi_slot_data_type         *slot_data_ptr              = NULL;
  mmgsdi_return_enum_type        mmgsdi_status              = MMGSDI_MAX_RETURN_ENUM;
  boolean                        is_icc_card                = FALSE;
  mmgsdi_technology_enum_type    request_tech               = MMGSDI_TECH_UNKNOWN;
  mmgsdi_attribute_enum_type     request_attr               = MMGSDI_ATTR_UNKNOWN;
  mmgsdi_attribute_enum_type     attr_iterator              = MMGSDI_ATTR_PRIMARY;

  MMGSDIUTIL_RETURN_IF_NULL_2(msg_ptr, mismatch_or_error_detected_ptr);

  UIM_MSG_HIGH_0("mmgsdi_session_deactivate_mismatch_sessions");

  /* Initializing this with TRUE. If we reach at the end of this function
     without any return in between then it means there is no ERROR and we
     don't need to wait for any de-activation (dest app is free). In that
     case set it to FALSE at the end so as to proceed with the activation
     in caller. */
  *mismatch_or_error_detected_ptr = TRUE;

  /* Determine the card type */
  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(msg_ptr->request_header.slot_id);
  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_is_card_icc(slot_data_ptr, &is_icc_card);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  /*=========================================================================
    DE-ACTIVATION ALGORITHM:

    1) First of all, by the time we reach here it is guranteed that the session
       requested in the activation request is free (already freed in
       mmgsdi_session_switch_provisioning_start() id active on other app)

    2) Check for the corresponding session of the requested session and if
       active on slot other than the requested SLOT than deactivate to keep
       sessions having same attribute (PRI/SEC/TER) as of req session mapped
       on same slot. Provide dummy callback here as we are not interested in
       the app mapped on this session.

    3) Go through the remaining 2 provisioning sessions (diff attributes and
       same technology).
       If anyone is active on slot same as requested SLOT then proceed with
       below steps.

       a) If the Technology is same and the card is ICC card then it means that
          the current session is active on same app in which we are interested
          because ICC cards have a single app for a tech area. Deactivate this
          session and provide the callback so that once this de-activation is
          complete we can proceed with the activation req. Also, deactivate
          the corresponding session by giving the dummy callback.
       c) If Tech is same and card is UICC, then compare the req aid and the
          aid on which session is active.
          - If same, means we are interested in this app. De-activate it and
            give callback fn from where we can proceed with activation once
            it is de-activated. Also, deactivate the corresponding session
          - If diff, means our dest app is free and the session is active on
            diff aid of same tech. No deactivation is required.

    NOTE: The de-activation of dest app should be done at the end so that by
          the time we hit the callback, other non-interested mismatch sessions
          will be de-active and we will not queue the de-activaon for them again.
  =========================================================================*/

  /* Getting the session type of the req session */
  mmgsdi_status = mmgsdi_util_get_session_type(msg_ptr->request_header.session_id,
                                               &request_session_type, NULL);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Unable to get session_type for session_id: 0x%x",
                  msg_ptr->request_header.session_id);
    return MMGSDI_ERROR;
  }

  /* Getting the session corresponding to the requested activation session */
  mmgsdi_status = mmgsdi_util_get_pair_session_type(request_session_type,
                                                    &pair_session_type);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Unable to get pair session type for req session type: 0x%x",
                  request_session_type);
    return MMGSDI_ERROR;
  }

  UIM_MSG_MED_2("Req session: 0x%x, Corres. req session: 0x%x",
                request_session_type, pair_session_type);

  /* Deactivation of corresponding source session if mapped on other slot
     which is not same as req slot. Here dummy callback is required as this
     is only corresponding session */
  mmgsdi_status = mmgsdi_util_get_mmgsdi_session_id_from_type(pair_session_type,
                                                              &pair_session_id);
  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    mmgsdi_slot_id_enum_type   slot_id   = MMGSDI_MAX_SLOT_ID_ENUM;

    mmgsdi_status = mmgsdi_util_get_session_app_info(pair_session_id,
                                                     &slot_id,
                                                     NULL, NULL, NULL, NULL, NULL);

    if (mmgsdi_status == MMGSDI_SUCCESS && slot_id != msg_ptr->request_header.slot_id)
    {
      mmgsdi_status = mmgsdi_session_deactivate_found_mismatch(msg_ptr,
                                                               pair_session_id,
                                                               FALSE);
      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        return mmgsdi_status;
      }
    }
  }

  mmgsdi_status = mmgsdi_util_get_tech_and_attr_from_session_type(request_session_type,
                                                                  &request_tech,
                                                                  &request_attr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Loop through all remaining 4 session with diff attr than the req session
     and if there is any mismatch as per above algo then de-activate */
  for (attr_iterator = MMGSDI_ATTR_PRIMARY; attr_iterator <= MMGSDI_ATTR_TERTIARY; attr_iterator++)
  {
    mmgsdi_technology_enum_type    tech_iterator    = MMGSDI_TECH_3GPP;

    /* Continue for same attribute session as already taken care.
       Also, continue for other tech session as we are checking for dest app
       that whether it is active on some other session or not which can
       only be possible on same tech.*/
    if (attr_iterator == request_attr)
    {
      continue;
    }

    for (tech_iterator = MMGSDI_TECH_3GPP; tech_iterator <= MMGSDI_TECH_3GPP2; tech_iterator++)
    {
      mmgsdi_session_type_enum_type current_session_type = MMGSDI_MAX_SESSION_TYPE_ENUM;
      mmgsdi_slot_id_enum_type      current_slot         = MMGSDI_MAX_SLOT_ID_ENUM;
      mmgsdi_session_id_type        curent_session_id    = MMGSDI_SESSION_ID_ZERO;

      if (tech_iterator != request_tech)
      {
        continue;
      }

      mmgsdi_status = mmgsdi_util_get_session_type_from_tech_and_attr(
                                                        tech_iterator,
                                                        attr_iterator,
                                                        &current_session_type);
      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        return mmgsdi_status;
      }

      mmgsdi_status = mmgsdi_util_get_mmgsdi_session_id_from_type(current_session_type,
                                                                  &curent_session_id);
      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        return mmgsdi_status;
      }

      mmgsdi_status = mmgsdi_util_get_session_app_info(curent_session_id,
                                                        &current_slot,
                                                       NULL, NULL, NULL, NULL, NULL);

      if (mmgsdi_status == MMGSDI_SUCCESS && current_slot == msg_ptr->request_header.slot_id)
      {
        /* If ICC card it means that this is dest app. Save the parameters
           so that the de-activate request can be queued at the end. */
        if (is_icc_card)
        {
          dest_app_session_id   = curent_session_id;
          dest_app_session_type = current_session_type;
        }
        else
        {
          mmgsdi_aid_type           app_data;
          mmgsdi_channel_info_type *channel_info_ptr = NULL;

          memset((void*)&app_data, 0x00, sizeof(mmgsdi_aid_type));

          if ((msg_ptr->app_id_data.data_len <= 0) ||
              (msg_ptr->app_id_data.data_len > MMGSDI_MAX_AID_LEN))
          {
            continue;
          }

          /* Copy the app. data from the request */
          if (msg_ptr->app_id_data.data_len > 0 &&
              msg_ptr->app_id_data.data_len <= MMGSDI_MAX_AID_LEN)
          {
            app_data.aid = msg_ptr->app_id_data;
          }

          mmgsdi_status = mmgsdi_util_get_session_and_channel_info(
                                                         curent_session_id,
                                                         NULL,
                                                         &channel_info_ptr);
          if (mmgsdi_status != MMGSDI_SUCCESS || channel_info_ptr == NULL)
          {
            continue;
          }
          if (channel_info_ptr != NULL && channel_info_ptr->app_info_ptr == NULL)
          {
            return MMGSDI_ERROR;
          }
          /* If comparison SUCCESS means this is dest app. Save the params
             to queue the de-activate req at the end */
          if (mmgsdi_util_cmp_aid(&channel_info_ptr->app_info_ptr->app_data,
                                  &app_data,
                                  FALSE) == MMGSDI_SUCCESS)
          {
            dest_app_session_id   = curent_session_id;
            dest_app_session_type = current_session_type;
          }
        }
      }
    }
  }

  /* If dest_app is active on some session then de-activate the corresponding
     session first and then deactivate the dest_app session at the end */
  if (dest_app_session_id != 0)
  {
    /* Getting the session corresponding to the requested activation session */
    mmgsdi_status = mmgsdi_util_get_pair_session_type(dest_app_session_type,
                                                      &pair_session_type);

    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_1("Unable to paired session type for dest app session type: 0x%x",
                    dest_app_session_type);
      return MMGSDI_ERROR;
    }

    UIM_MSG_MED_2("Destination App session: 0x%x, Corres. Session: 0x%x",
                  dest_app_session_type, pair_session_type);

    /* Deactivation of corresponding session if mapped on same slot where
       we received the activation req. Here dummy callback is required as this
       is only corresponding session */
    mmgsdi_status = mmgsdi_util_get_mmgsdi_session_id_from_type(pair_session_type,
                                                                &pair_session_id);
    if (mmgsdi_status == MMGSDI_SUCCESS)
    {
      mmgsdi_slot_id_enum_type   slot_id   = MMGSDI_MAX_SLOT_ID_ENUM;

      mmgsdi_status = mmgsdi_util_get_session_app_info(pair_session_id,
                                                        &slot_id,
                                                       NULL, NULL, NULL, NULL, NULL);

      if (mmgsdi_status == MMGSDI_SUCCESS && slot_id == msg_ptr->request_header.slot_id)
      {
        mmgsdi_status = mmgsdi_session_deactivate_found_mismatch(msg_ptr,
                                                                 pair_session_id,
                                                                 FALSE);
        if (mmgsdi_status != MMGSDI_SUCCESS)
        {
          return mmgsdi_status;
        }
      }
    }

    /* Deactivate the destination app now */
    mmgsdi_status = mmgsdi_session_deactivate_found_mismatch(msg_ptr,
                                                             dest_app_session_id,
                                                             TRUE);
    return mmgsdi_status;
  }

  /* Update this falg to FALSE if no mis-match is found so that
     in caller we can go ahead with our activation process */
  *mismatch_or_error_detected_ptr = FALSE;
  return MMGSDI_SUCCESS;
} /* mmgsdi_session_deactivate_mismatch_sessions */


/*===========================================================================
  FUNCTION MMGSDI_SESSION_PRE_PROV_ACTIVATION_START

DESCRIPTION
  If skip_app_termination feature is enabled ,
      a. Checks if the activation is a re-activation of an application which is
         previously deactivated and the channel can be re-used
      b. If yes , updates the channel information for session and updates the
         skip app terminated globals
      c. If no, starts an activation sequence for the requested application afresh

  If skip_app_termination feature is disabled ,
      Starts an activation sequence for the requested application afresh

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_session_pre_prov_activation_start(
                                                   mmgsdi_session_act_or_switch_prov_req_type  *msg_ptr)
{
  uint8                     channel_info_index = MMGSDI_MAX_CHANNEL_INFO;
  mmgsdi_sw_status_type     sw_status          = {0,0};
  mmgsdi_return_enum_type   mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_session_info_type  *session_ptr       = NULL;
  boolean                   halt_activation    = TRUE;

  MMGSDIUTIL_RETURN_IF_NULL(msg_ptr);

  mmgsdi_status = mmgsdi_util_get_session_and_channel_info(msg_ptr->request_header.session_id,
                                                           &session_ptr,
                                                           NULL);

  if( mmgsdi_status != MMGSDI_SUCCESS )
  {
    return mmgsdi_status;
  }

  if( session_ptr == NULL )
  {
    return MMGSDI_ERROR;
  }

  /* Check if there are any mis-matches. If yes, de-activate them and return.
     In case, destination app is active wait for callback to be called and from
     there we will again move ahead with our activation procedure. If there
     are no mis-matches or ERROR (halt_activation == FALSE) then go ahead
     with activation. If there is any ERROR occured while deactivation then
     return to stop activation process as per our normal ERROR handling */
  mmgsdi_status = mmgsdi_session_deactivate_mismatch_sessions(msg_ptr,
                                                              &halt_activation);

  if (mmgsdi_status != MMGSDI_SUCCESS || halt_activation != FALSE)
  {
    return mmgsdi_status;
  }

  if((mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_SKIP_PROV_APP_TERMINATION, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) ==
        MMGSDI_FEATURE_ENABLED) &&
     (mmgsdi_util_check_is_channel_available_for_reuse_uicc(
        msg_ptr->request_header.slot_id, &msg_ptr->app_id_data, NULL, &channel_info_index)) &&
     (channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
     (mmgsdi_channel_info_ptr_table[channel_info_index] != NULL) &&
      mmgsdi_channel_info_ptr_table[channel_info_index]->app_termination_skipped)
  {
    UIM_MSG_HIGH_2("Activating app whose termination is skipped during deactivation.Channel 0x%x Slot 0x%x",
                    channel_info_index,
                    msg_ptr->request_header.slot_id);

    MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

    /* Update the channel index information to the session pointer and
       increment the connection count of that index by 1 */
    session_ptr->channel_info_index = channel_info_index;
    mmgsdi_channel_info_ptr_table[channel_info_index]->num_connections++;

    mmgsdi_status = mmgsdi_session_populate_session_info_act_prov(session_ptr);

    /* app_termination_skipped flag can be set to FALSE, since the app activaion has started
       and any further session deactivation or card error should be handled appropriately. */
    mmgsdi_channel_info_ptr_table[channel_info_index]->app_termination_skipped = FALSE;

    /*  And, since the app was not terminated earlier, few activation steps can be skipped -
        for eg sending STATUS command before sub ready */
    mmgsdi_channel_info_ptr_table[channel_info_index]->selective_act_after_app_term_skipped = TRUE;

    MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

    /* As part of processing of the confirmation a SESSION_CHANGED event
       will be triggered after which app initialization will start. */
    mmgsdi_status =  mmgsdi_cnf_build_and_queue(mmgsdi_status,
                                                &msg_ptr->request_header,
                                                NULL,
                                                NULL,
                                                FALSE,
                                                sw_status);
  }
  else
  {
    /* We can be here for following two reasons
       1. If skip prov app termination feature is DISABLED
       2. If skip prov app termination feature is ENABLED
          a. The app to be activated is first time activated on a session already tied to another application
          b. The channel could not be re-used for this activation for some reason */
    UIM_MSG_HIGH_0("Start Activation of the app normally");
    mmgsdi_status = mmgsdi_session_activate_provisioning_start(msg_ptr);
  }
  return mmgsdi_status;
} /*mmgsdi_session_pre_prov_activation_start*/

