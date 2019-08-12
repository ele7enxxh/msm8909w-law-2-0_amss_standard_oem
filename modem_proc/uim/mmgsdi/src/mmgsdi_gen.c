/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    M M G S D I   I C C   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the Generic commands that are not protocol dependent

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_gen.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/24/16   ar      Review of macros used by MMGSDI
05/03/16   tkl     F3 log prints cleanup
10/06/15   tl      Add backup to Get Response APDU in case not in cache
09/07/15   bcho    Migrate to mcfg EFS APIs
09/08/15   bcho    Run SIMLOCK Algo in Onchip SIM 
07/09/15   ar      Validate PRL at diff steps if SD task not ready
11/05/14   ar      Remove mmgsdi_get_card_reader_status () API
10/28/14   vv      Do not run simlock algo on a single slot device
10/13/14   tl      Fix compiler warnings
09/09/14   kk      Enhancements in NV handling
09/27/14   ar      Add support for multiple slots in BT-SAP
09/25/14   tl      Add SimLock featurization to run algo
08/27/14   tl      Introduce network related SIM Lock slot policies
08/06/14   ar      Replace MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE with new macro
07/25/14   av      Auto onchip activation for multi slots
07/15/14   yt      Support for NV refresh
07/10/14   tl      Remove ALWAYS OFF feature FEATURE_MMGSDI_PKCS15
06/30/14   vv      Remove Perso code
03/18/14   tl      Introduce new SIM Lock feature
02/06/14   tl      Check slot before updating channel state when streaming apdu
01/11/14   df      Fix off-target compilation error
01/10/14   df      Use v2.0 diag macros
01/09/14   ar      Replace old Macros with MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE
12/11/13   df      Remove unused memory paging feature
12/01/13   df      Convert to use prov session type to session ID util
11/18/13   vv      Added support for parallel REFRESH
10/16/13   df      Removing legacy gsdi conversion function calls
10/04/13   rp      F3 message reduction
09/26/13   av      Enable OnChip on bootup based on EFS item
08/30/13   tl      Convert channel info table from static to dynamic allocation
06/24/13   vdc     Added support for triple SIM
06/04/13   yt      Fix Klocwork errors
05/30/13   at      Fix to skip 6C XX for GET RESPONSE of P3=0
05/28/13   spo     Replaced instances of memcpy with memscpy
05/16/13   vdc     Replace memcpy with safer version memscpy
05/08/13   at      Supporting 62XX & 63XX warnings in APDU transactions
05/06/13   spo     Memory optimization for get response Cache
03/04/13   av      Updates for T3245 timer support to mark APP as legal/valid
02/04/13   vv      Added mmgsdi_session_read_prl support for RUIM card
12/05/12   vv      Added support for parallel processing in MMGSDI
10/11/12   spo     Removed unnecessary F3 messages
10/03/12   abg     Removed Queuing related F3 messages
10/01/12   abg     Removed Problem determining protocol type F3 message
09/28/12   abg     Updated ERR to MSG_ERROR
09/05/12   av      Handling of VPE response from the card
07/25/12   bcho    Set channel status to READY_WITH_STRMG while sending APDU to
                   UIM
05/22/12   shr     Add support for variable length Send APDU response
03/30/12   av      Replaced feature flags with NVITEM
03/29/12   shr     Added support for Send APDU extension API
03/01/12   kk      Added support for app state extraction in get_session_info
03/01/12   bcho    Add check for app type CSIM and app state READY in function
                   mmgsdi_gen_session_read_prl
02/18/12   av      Cleaned up remaining ALWAYS OFF features
01/14/12   bcho    Null check for app info ptr in function
                   mmgsdi_gen_session_illegal_sim
01/09/12   js      Reset sanity timer value on card error
12/21/11   shr     Legacy GSDI removal updates
10/13/11   shr     Adding support for full recovery
09/06/11   shr     Do not attempt a Card Reset as part of EFRESH RESET/Card
                   power-up handling, if the REFRESH has been stopped
08/22/11   yt      Send CARD_PDOWN to UIM without checking for MMGSDI state
08/18/11   av      Fixes for MMGSDI Klocwork CRITICAL warnings
05/19/11   at      Ignore card pdown on no card present or card error
05/17/11   shr     Updates for Thread safety
01/12/11   yt      Fixed compiler warnings
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
10/14/10   yt      Updated handling of client req table entries in case of error
10/05/10   nmb     Removed extra bytes from task_cmd buffer
09/15/10   nmb     Allocate mmgsdi_client_id_reg_table dynamically
08/30/10   shr     Perso clean up for Sessions
08/09/10   yt      Fixed compiler warnings
07/14/10   js      Fixed compilation warnings
07/09/10   tkl     Fixed enum mapping between mmgsdi_perso_feature_enum_type
                   & gsdi_perso_enum_type
07/01/10   nb      Added Support for 2G send apdu command
06/25/10   shr     Perso commands need to be allowed when no Card is inserted
06/24/10   yt      Klocwork fixes
06/19/10   vs      Illegal SIM req post-processing
05/03/10   shr     Added Dual Slot support for REFRESH RESET and REFRESH
                   RESET_AUTO
03/12/10   adp     Fixing KW errors
03/09/10   shr     Clean up REFRESH FCN, INIT, INIT_FCN and APP_RESET
03/02/10   kk      Fixed get all available apps command handling
02/19/10   shr     Session get info API implementation
02/01/10   jk      EPRL Support
12/18/09   nb      Moved Dual Slot Changes
12/17/09   kk      Added app state check for Perso handling
09/26/09   kp      ZI memory reduction changes
07/28/09   mib     Added mmgsdi_perso_lock_down_and_enable_limited_access
05/15/09   nb      Added support for perso_get_unblock_dck_retries command
03/24/09   nb      Fixed problem with PERSO_UNBLOCK command
02/24/09   nb      Perso Command support in MMGSDI
11/18/08   js      Updated featurization for Send APDU related code
11/15/08   js      Removed MFLO featurization for Send APDU related code
10/20/08   sun     Allow protocol to be sent as part of Card Reset
09/16/08   kk      Updated arg to mmgsdi_util_queue_mmgsdi_uim_report_rsp in
                   create pkcs15 command handler
09/06/08   kk      Changed pkcs15 command handling
08/28/08   sun     If command fails in the original state, then return original
                   error mmgsdi status to client
11/15/07   sun     Added support for NAA refresh
09/27/07   tml     Lint fixes
09/10/07   tml     Added MFLO Supports.
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
01/17/07   sp      Removed pin_just_verified flag
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
06/19/06   sun     Fixed Slot for Refresh
04/18/06   nk      featurized file and converted macro calls to function calls
03/21/06   tml     lint
03/14/06   sun     If the refresh fails , then remember to refresh
                   after the conditions are satisified
02/23/05   tml     Assign a discard cnf cb pointer to avoid overwriting
                   gsdilib cmd q during the refresh reset process.
12/14/05   tml     MMGSDI Cleanup
11/09/05   sun     Fixed Lint Errors
11/03/05   tml     Fixed header
11/03/05   sun     Changed Refresh to Refresh Command
08/25/05   pv      Initial version
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "intconv.h"
#include "mmgsdi.h"
#include "uim.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "mmgsdi_gen.h"
#include "mmgsdi_uim_uicc.h"
#include "mmgsdi_uim_icc.h"
#include "mmgsdi_evt.h"
#include "mmgsdi_cnf.h"
#include "mmgsdi_nv.h"
#include "fs_sys_types.h"
#include "fs_public.h"
#include "uim_common_efs.h"
#include "mmgsdi_simlock.h"
#include "mmgsdi_card_init.h"
#include "mmgsdi_efs.h"

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/* Offset for GET RESPONSE APDU Len */
#define MMGSDI_APDU_GET_RESPONSE_LEN_OFFSET 4

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


/* =============================================================================
   FUNCTION:      MMGSDI_GEN_CARD_RESET_SYNC

   DESCRIPTION:
     This function shall send a reset command to UIM and wait for the response
     synchronously

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
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_card_reset_sync(
  mmgsdi_slot_id_enum_type slot,
  uim_protocol_type        protocol)
{
  uim_cmd_type *          uim_cmd_ptr   = NULL;
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;
  uim_slot_type           uim_slot      = UIM_SLOT_NONE;
  mmgsdi_slot_data_type  *slot_data_ptr = NULL;
  mmgsdi_task_enum_type   mmgsdi_task   = MMGSDI_TASK_MAX;

  /* Convert to UIM slot */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(slot, &uim_slot);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot);
  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* Get the current task */
  MMGSDIUTIL_GET_CURRENT_MMGSDI_TASK(&mmgsdi_task);
  if(mmgsdi_task == MMGSDI_TASK_MAX)
  {
    UIM_MSG_HIGH_0("MMGSDI task is undetermined");
  }

  /* Get a UIM buffer for the request */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type));

  if (uim_cmd_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* Populate UIM information */
  uim_cmd_ptr->hdr.protocol = protocol;
  uim_cmd_ptr->hdr.channel  = UIM_CHANNEL0;
  uim_cmd_ptr->hdr.command  = UIM_RESET_F;
  uim_cmd_ptr->hdr.slot     = uim_slot;

  /* Reset Sanity Timer to default value */
  slot_data_ptr->mmgsdi_uim_sanity_timer_value
    = MMGSDI_UIM_SANITY_TIMER_VALUE;

  /* Reset GSTK loc envelope (VPE) response count */
  mmgsdi_util_reset_gstk_loc_env_rsp_count(slot);

  /* Send the command to UIM synchronously */
  mmgsdi_status = mmgsdi_send_cmd_to_uim_server_synch(uim_cmd_ptr);

  /* Clear any memory that has been allocated */
  if (mmgsdi_task != MMGSDI_TASK_MAX &&
      mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task] != NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
      mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]->rsp_data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]);
  }

  /* Free the UIM command pointer that has been allocated */
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);

  return mmgsdi_status;
} /* mmgsdi_gen_card_reset_sync */


/* =============================================================================
   FUNCTION:      MMGSDI_GEN_PROCESS_CARD_PUP

   DESCRIPTION:
     This function shall do the procedures similar to SIM REFRESH.  There by
     sending a reset command to UIM.

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

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_process_card_pup(
  const mmgsdi_card_pup_req_type  *req_ptr)
{
  mmgsdi_return_enum_type            mmgsdi_status   = MMGSDI_SUCCESS;

  /*
   * Parameter checks.
   */
  MMGSDIUTIL_RETURN_IF_NULL_2(req_ptr, req_ptr->request_header.response_cb);

  if (req_ptr->option == MMGSDI_CARD_POWER_UP_INITIAL_PUP)
  {
    UIM_MSG_ERR_0("Should not handle CardPower Up Initial PUP Option in this function");
    return MMGSDI_ERROR;
  }
  else
  {
    uim_cmd_type*                      uim_cmd_ptr     = NULL;
    uim_slot_type                      uim_slot        = UIM_SLOT_NONE;
    int32                              index           = 0;
    mmgsdi_client_req_extra_info_type* extra_param_ptr = NULL;

    /*
     * Convert to UIM slot.
     */
    mmgsdi_status = mmgsdi_util_convert_uim_slot(
                      req_ptr->request_header.slot_id, &uim_slot);

    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }
    /*
     * Allocate and populate extra parameter.
     */
    mmgsdi_status =  mmgsdi_util_alloc_and_populate_extra_param(
                       MMGSDI_CARD_PUP_REQ, (void*)req_ptr, &extra_param_ptr);

    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }
    /*
     * 1) Get a UIM buffer for the request.
     * 2) Check for if the buffer is NULL or not.
     */
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type));

    if (uim_cmd_ptr == NULL)
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    /*
     * Populate UIM information.
     */
    uim_cmd_ptr->hdr.command  = UIM_POWER_UP_UIM_PASSIVE_F;
    uim_cmd_ptr->hdr.protocol = UIM_NO_SUCH_MODE;
    uim_cmd_ptr->hdr.slot     = uim_slot;
    /*
     * Get a new index from client req info table for response data.
     * Set the uim data pointer to the client request table data buffer.
     */
    mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
      return mmgsdi_status;
    }
    mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                      index,
                      &req_ptr->request_header,
                      uim_cmd_ptr,
                      extra_param_ptr);

    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      mmgsdi_util_free_client_request_table_index(index);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
      return mmgsdi_status;
    }
    /*
     * Assign uim_cmd_ptr's user data.
     */
    uim_cmd_ptr->hdr.user_data = int32touint32(index);
    UIM_MSG_HIGH_0("Sending down card power up command to UIM");
    mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      mmgsdi_util_free_client_request_table_index(index);
    }
  }
  return mmgsdi_status;
} /* mmgsdi_gen_process_card_pup */


/* =============================================================================
   FUNCTION:      MMGSDI_GEN_PROCESS_CARD_PDOWN

   DESCRIPTION:
     This function will send down a power down command to the UIM
     module to power down the CARD.

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

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_process_card_pdown(
  const mmgsdi_card_pdown_req_type *req_ptr)
{
  uim_cmd_type*                      uim_cmd_ptr     = NULL;
  uim_slot_type                      uim_slot        = UIM_SLOT_NONE;
  mmgsdi_return_enum_type            mmgsdi_status   = MMGSDI_SUCCESS;
  int32                              index           = 0;
  mmgsdi_client_req_extra_info_type* extra_param_ptr = NULL;

  /*
   * Parameter checks.
   */
  MMGSDIUTIL_RETURN_IF_NULL_2(req_ptr, req_ptr->request_header.response_cb);
  /*
   * Convert to UIM slot.
   */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(
                    req_ptr->request_header.slot_id, &uim_slot);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }
  /*
   * Allocate and populate extra parameter.
   */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
                        MMGSDI_CARD_PDOWN_REQ,
                        (void*)req_ptr, &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }
  /*
   * 1) Get a UIM buffer for the request.
   * 2) Check for if the buffer is NULL or not.
   */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type));

  if (uim_cmd_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*
   * Populate UIM information.
   */
  uim_cmd_ptr->hdr.command        = UIM_POWER_DOWN_F;
  uim_cmd_ptr->hdr.protocol       = UIM_NO_SUCH_MODE;
  uim_cmd_ptr->hdr.slot           = uim_slot;

   /* Populate the power down option to UIM*/
  if (req_ptr->option == MMGSDI_CARD_POWER_DOWN_NOTIFY_GSDI)
  {
    uim_cmd_ptr->pdown.pdown_option = UIM_OPTION_NOTIFY_PDOWN;
  }
  else
  {
    uim_cmd_ptr->pdown.pdown_option = UIM_OPTION_NO_NOTIFY_PDOWN;
  }

  /*
   * Get a new index from client req info table for response data.
   * Set the uim data pointer to the client request table data buffer.
   */
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }
  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                    index,
                    &req_ptr->request_header,
                    uim_cmd_ptr,
                    extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }
  /*
   * Assign uim_cmd_ptr's user data.
   */
  uim_cmd_ptr->hdr.user_data = int32touint32(index);
  UIM_MSG_HIGH_0("Sending POWER down command to UIM");
  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
  }
  return mmgsdi_status;
} /* mmgsdi_gen_process_card_pdown */


/* =============================================================================
   FUNCTION:      MMGSDI_GEN_PROCESS_CARD_RESET

   DESCRIPTION:
     This function shall do the procedures similar to SIM REFRESH.  There by
     sending a reset command to UIM.

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

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_process_card_reset(
  const mmgsdi_card_reset_req_type  *req_ptr)
{
  mmgsdi_return_enum_type            mmgsdi_status   = MMGSDI_SUCCESS;
  uim_cmd_type*                      uim_cmd_ptr     = NULL;
  uim_slot_type                      uim_slot        = UIM_SLOT_NONE;
  int32                              index           = 0;
  mmgsdi_sw_status_type              status_word     = {0};
  mmgsdi_slot_data_type             *slot_data_ptr   = NULL;
  uint8                              slot_index      = MMGSDI_SLOT_1_INDEX;

  /*
   * Parameter checks.
   */
  MMGSDIUTIL_RETURN_IF_NULL_2(req_ptr, req_ptr->request_header.response_cb);

  mmgsdi_status = mmgsdi_util_get_slot_index(req_ptr->request_header.slot_id,
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

  /* If the Card Reset is triggered due to a REFRESH (Refresh Reset/Card
     power-up), then proceed with queuing a Reset request to UIM only if
     the REFRESH is still in progress. A Card Error for example could have
     stopped the REFRESH and in this case, do not queue the Reset request */
  if((req_ptr->reset_cause == MMGSDI_RESET_CAUSE_REFRESH) &&
     ((mmgsdi_refresh_info_ptr[slot_index] == NULL) ||
      (mmgsdi_refresh_info_ptr[slot_index]->request_header.slot_id !=
       req_ptr->request_header.slot_id)))
  {
    UIM_MSG_ERR_0("Cannot proceed with Card Reset, REFRESH no longer in progress");

    /* Reporting the Card Reset as Success - a failure status would
       lead to any new REFRESH ongoing to be stopped in
       mmgsdi_internal_refresh_cb() */
    mmgsdi_status = mmgsdi_cnf_build_and_queue(MMGSDI_SUCCESS,
                                               &req_ptr->request_header,
                                               NULL,
                                               NULL,
                                               FALSE,
                                               status_word);

    return mmgsdi_status;
  }

  /*
   * Convert to UIM slot.
   */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(
                    req_ptr->request_header.slot_id, &uim_slot);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(req_ptr->request_header.slot_id);
  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /*
   * 1) Get a UIM buffer for the request.
   * 2) Check for if the buffer is NULL or not.
   */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type));
  if (uim_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*
   * Populate UIM information.
   */
  uim_cmd_ptr->hdr.protocol = req_ptr->protocol;
  uim_cmd_ptr->hdr.channel  = UIM_CHANNEL0;
  uim_cmd_ptr->hdr.command  = UIM_RESET_F;
  uim_cmd_ptr->hdr.slot     = uim_slot;
  /*
   * Get a new index from client req info table for response data.
   * Set the uim data pointer to the client request table data buffer.
   */
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }
  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                    index,
                    &req_ptr->request_header,
                    uim_cmd_ptr,
                    NULL);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }
  /*
   * Assign uim_cmd_ptr's user data.
   */
  uim_cmd_ptr->hdr.user_data = int32touint32(index);

  /* Reset Sanity Timer to default value */
  slot_data_ptr->mmgsdi_uim_sanity_timer_value
      = MMGSDI_UIM_SANITY_TIMER_VALUE;

  UIM_MSG_HIGH_0("Sending down reset command to UIM");
  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
  }
  return mmgsdi_status;
} /* mmgsdi_gen_process_card_reset */


/*===========================================================================
FUNCTION:      MMGSDI_GEN_BUILD_CARD_RESET

DESCRIPTION:
  This function is used to build a card reset command

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
  Will result in your callback getting called everytime MMGSDI Needs to notify
  any client of any events.
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_build_card_reset(
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           slot_id,
  mmgsdi_cmd_enum_type               curr_req_type,
  uim_protocol_type                  protocol,
  mmgsdi_card_reset_cause_enum_type  reset_cause,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref)
{
  mmgsdi_task_cmd_type                 *task_cmd_ptr       = NULL;
  mmgsdi_return_enum_type               mmgsdi_status      = MMGSDI_SUCCESS;
  mmgsdi_card_reset_req_type           *card_reset_ptr     = NULL;
  int32                                 total_mem_len      = 0;
  uint32                                task_cmd_len       = 0;
  mmgsdi_slot_data_type                *slot_data_ptr      = 0;

  UIM_MSG_HIGH_1("MMGSDI_GEN_BUILD_CARD_RESET, client_id 0x%x", client_id);

  /* If Onchip is activated, do not queue card reset command and send an ERROR
     response. If we return SUCCESS here, the caller might perform the mmgsdi
     clean up or continue with refresh procedure which we would mess up the
     Onchip configuration */
  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);

  if(slot_data_ptr != NULL &&
     slot_data_ptr->onchip_sim_data.state == MMGSDI_ONCHIP_SIM_INIT)
  {
    UIM_MSG_HIGH_0("Onchip is activated; build_card_reset ignored");
    return MMGSDI_ERROR;
  }

  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  task_cmd_len = int32touint32(total_mem_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /*---------------------------------------------------------------------------
    - Align pointers of the various structures members
  ---------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.card_reset_req,
                         curr_req_type);

  card_reset_ptr = &task_cmd_ptr->cmd.cmd.card_reset_req;

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
  task_cmd_ptr->cmd.cmd_enum                       = curr_req_type;
  card_reset_ptr->request_header.client_id         = client_id;
  card_reset_ptr->request_header.request_type      = curr_req_type;
  card_reset_ptr->request_header.orig_request_type = curr_req_type;
  card_reset_ptr->request_header.slot_id           = slot_id;
  card_reset_ptr->request_header.client_data       = client_ref;
  card_reset_ptr->request_header.response_cb       = response_cb_ptr;
  card_reset_ptr->request_header.request_len       = total_mem_len;
  card_reset_ptr->request_header.payload_len       = card_reset_ptr->request_header.request_len -
                                                          uint32toint32(sizeof(mmgsdi_request_header_type));

  card_reset_ptr->protocol    = protocol;
  card_reset_ptr->reset_cause = reset_cause;

  /* PLACE ON GSDI QUEUE */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS) {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(task_cmd_ptr);
    return mmgsdi_status;
  }
  /* Reset GSTK loc envelope (VPE) response count */
  mmgsdi_util_reset_gstk_loc_env_rsp_count(slot_id);
  return mmgsdi_status;
} /* mmgsdi_gen_build_card_reset */


/*=============================================================================
  FUNCTION: MMGSDI_GEN_PROCESS_GET_ATR

  DESCRIPTION:
    Function will get the ATR (Answer to Reset)from the UIM.

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    mmgsdi_return_enum_type

  SIDE EFFECTS
    NONE
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_process_get_atr (
  mmgsdi_cmd_enum_type           cmd,
  const mmgsdi_get_atr_req_type* req_ptr
)
{
  mmgsdi_return_enum_type            mmgsdi_status   = MMGSDI_SUCCESS;
  int32                              index           = 0;
  uint8                              slot_index      = 0;

  /* --------------------------------------------------------------------------
     Validate the Input Parameters checks.
     ----------------------------------------------------------------------- */
  MMGSDIUTIL_RETURN_IF_NULL_2(req_ptr, req_ptr->request_header.response_cb);

  mmgsdi_status = mmgsdi_util_get_slot_index(req_ptr->request_header.slot_id,
                                             &slot_index);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if (cmd == MMGSDI_SAP_GET_ATR_REQ && !mmgsdi_sap_connected[slot_index])
  {
    UIM_MSG_ERR_0("SAP GET ATR Req error: BT is not connected");
    return MMGSDI_ERROR;
  }

  /* No Extra Param */

  /* --------------------------------------------------------------------------
     Get an index from the table used to store client information.
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* --------------------------------------------------------------------------
     Store the Client Information for later use when the Response is provided
     by the UIM
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_populate_client_request_table_info( index, &req_ptr->request_header,
                                                                  NULL,
                                                                  NULL);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }

  /* --------------------------------------------------------------------------
     Store the Client Information for later use when the Response is provided
     by the UIM
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(
                    index, cmd, MMGSDI_SUCCESS);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_gen_process_get_atr */


/*===========================================================================
FUNCTION MMGSDI_GEN_RSP_TABLE_RETRIEVE

DESCRIPTION
  Function will retrieve the APDU Response Data cached from the previous
  applicable APDU Exchange with the Card inserted.

DEPENDENCIES
  The APDU_PTR provided must contain a valid pointer and all memory must
  be appropriately allocated to match the data maintained in this table.

  A requested length less than or equal to the amount of data
  to retrieve will be acceptable.

  A request length exceeding the amount of data to retrive will be unacceptable
  and will be handled by an error to the client.

RETURN VALUE
  mmgsdi_return_enum_type:
  MMGSDI_SUCCESS:           Get Response Data populated in the apdu_ptr
                            provided.
  MMGSDI_NOT_FOUND:         Get Response Data for the CLIENT ID, SLOT ID,
                            CHANNEL ID is no longer present
  MMGSDI_INCORRECT_PARAMS:  Parameter check failure. NULL DATA PTR provided
                            or Get Response Value exceeds the maximum.

SIDE EFFECTS
  None.
===========================================================================*/
static mmgsdi_return_enum_type  mmgsdi_gen_get_rsp_table_retrieve(
  mmgsdi_client_id_type      client_id,
  mmgsdi_slot_id_enum_type   slot_id,
  uint8                      channel_id,
  int32                      req_data_len,
  mmgsdi_data_type          *apdu_ptr,
  uint8                     *sw_ptr
)
{
  mmgsdi_return_enum_type status     = MMGSDI_SUCCESS;
  int32                   tbl_index  = 0;
  mmgsdi_len_type         apdu_len   = 0;

  UIM_MSG_HIGH_3("GET APDU Get Response: CLIENT 0x%x SLOT 0x%x CH ID 0x%x",
                 client_id, slot_id, channel_id);

  /* --------------------------------------------------------------------------
     Validate the input Parameters Provided
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL_3(apdu_ptr, sw_ptr, apdu_ptr->data_ptr);
  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(req_data_len,0x00,MMGSDI_GEN_MAX_GET_RESPONSE_REQ_LEN);

  /* --------------------------------------------------------------------------
     Search the APDU Response Cache Data by using the following matching
     algorithm.  Match CLIENT_ID.  Then Match SLOT_ID.  Then Match CHANNEL_ID
     ----------------------------------------------------------------------- */
  for (tbl_index = 0; tbl_index < MMGSDI_MAX_AVAIL_GET_RSP_CACHE_ENTRIES; tbl_index++)
  {
    if(mmgsdi_apdu_get_rsp_table_ptr[tbl_index] == NULL)
    {
      continue;
    }

    if (mmgsdi_apdu_get_rsp_table_ptr[tbl_index]->client_id != client_id )
    {
      /* Client IDs did not match for the client */
      continue;
    }

    if (mmgsdi_apdu_get_rsp_table_ptr[tbl_index]->slot_id != slot_id )
    {
      /* Not the correct Slot ID for which APDU Data is requested
      ** Client may be sending APDUs over various SLOT IDs.
      ** Continue Searching */
      continue;
    }

    if (mmgsdi_apdu_get_rsp_table_ptr[tbl_index]->channel_id != channel_id )
    {
      /* Not the Channel ID for which APDU Data requested
      ** Client may be sending APDUs over various SLOT IDs using
      ** different channels */
      continue;
    }
    /* Break out of the for loop.  Have a match */
    break;
  }

  if (tbl_index >= MMGSDI_MAX_AVAIL_GET_RSP_CACHE_ENTRIES)
  {
    /* No client ID match */
    MMGSDIUTIL_DEBUG_MSG_ERROR_1("Invalid tbl_index 0x%x ", tbl_index);
    return MMGSDI_ERROR;
  }

  /* At this point, the Client ID Matches, the Slot ID Matches
  ** and the channel id matches.  This is the correct APDU Data
  ** Copy the data over to the requester's buffer after a few
  ** length checks to ensure we can protect against buffer overrun*/
  apdu_len = mmgsdi_apdu_get_rsp_table_ptr[tbl_index]->apdu.data_len;


  UIM_MSG_HIGH_2("Req. length = 0, Get Resp. cache total length 0x%x",
                 req_data_len, apdu_len);

  /* If valid Get Response data is available in the cache */
  if (apdu_len > MMGSDI_GEN_SW_LEN)
  {
    /* In case of warning status words, we expect the client to send a
       GET RESPONSE with Le = 0, per TS 102.221, Section C.1.7
       If data available is < 256, send intermediate 6C XX as SW bytes
       If data available is >= 256, skip sending intermediate 6C XX SW bytes,
        instead send data directly */
    if ((mmgsdi_apdu_get_rsp_table_ptr[tbl_index]->implicit_sw1 == GSDI_SW1_WARNINGS_PART_1 ||
         mmgsdi_apdu_get_rsp_table_ptr[tbl_index]->implicit_sw1 == GSDI_SW1_WARNINGS_PART_2) &&
        (req_data_len == 0x00) &&
        (apdu_len <= MMGSDI_GEN_MAX_GET_RESPONSE_REQ_LEN + MMGSDI_GEN_SW_LEN))
    {
      /* Now update the SW1 & SW2 */
      apdu_ptr->data_len = 0;
      sw_ptr[0] = GSDI_SIM_SW1_PROCEDURE_BYTE_6C;
      sw_ptr[2] = GSDI_SIM_SW1_PROCEDURE_BYTE_6C;

      /* Update implicit SW2 with the pending Get Response data length */
      sw_ptr[1] =  int32touint8(
                       mmgsdi_apdu_get_rsp_table_ptr[tbl_index]->apdu.data_len -
                       MMGSDI_GEN_SW_LEN);
      sw_ptr[3] = sw_ptr[1];
      return MMGSDI_SUCCESS;
    }

    /* If 256 bytes or above of Get Response data is available */
    if(apdu_len > MMGSDI_GEN_MAX_GET_RESPONSE_REQ_LEN + MMGSDI_GEN_SW_LEN)
    {
      if ( req_data_len == 0)
      {
        /* Return 256 bytes of Get Resp. data when requested data length is 0 */
        req_data_len = MMGSDI_GEN_MAX_GET_RESPONSE_REQ_LEN + 1;
      }
    }
    else
    {
      if ( req_data_len > (apdu_len - MMGSDI_GEN_SW_LEN))
      {
        /* Length Requested exceeds length cached */
        UIM_MSG_ERR_2("Req Length Exceed Max, Req 0x%x Avail 0x%x",
                      req_data_len, (apdu_len - MMGSDI_GEN_SW_LEN));

        status = MMGSDI_ERROR;
      }
      else if ( req_data_len == 0)
      {
        /* Return entire cached Get Resp. data when requested data length is 0 */
        req_data_len = apdu_len - MMGSDI_GEN_SW_LEN;
      }
    }
    if (status == MMGSDI_SUCCESS )
    {
      /* Store the contents into the buffer provided by the
      ** Requester
      */
      mmgsdi_memscpy(apdu_ptr->data_ptr,
        MMGSDI_GEN_MAX_GET_RESPONSE_REQ_LEN + 1,
        mmgsdi_apdu_get_rsp_table_ptr[tbl_index]->apdu.data_ptr,
        (size_t)req_data_len);

      apdu_ptr->data_len = req_data_len;

      /* Update the APDU cache data length */
      mmgsdi_apdu_get_rsp_table_ptr[tbl_index]->apdu.data_len =
        apdu_len - req_data_len;

      /* If Get Response data is still available in cache */
      if(mmgsdi_apdu_get_rsp_table_ptr[tbl_index]->apdu.data_len >
         MMGSDI_GEN_SW_LEN)
      {
         /* Move the remaining data to the start of the data buffer */
        mmgsdi_memsmove(mmgsdi_apdu_get_rsp_table_ptr[tbl_index]->apdu.data_ptr,
          apdu_len,
          &mmgsdi_apdu_get_rsp_table_ptr[tbl_index]->apdu.data_ptr[req_data_len],
          (apdu_len - req_data_len));

        /* Return Implicit SW1 (0x61/0x9F) back to client to indicate more
           data is pending to be Fetched */
        sw_ptr[0] = mmgsdi_apdu_get_rsp_table_ptr[tbl_index]->implicit_sw1;
        sw_ptr[2] = mmgsdi_apdu_get_rsp_table_ptr[tbl_index]->implicit_sw1;

        /* If pending Get Response data length exceeds 0xFF, then return
           implicit SW2 as 0x00 to indicate the same */
        if(mmgsdi_apdu_get_rsp_table_ptr[tbl_index]->apdu.data_len >
           MMGSDI_GEN_MAX_GET_RESPONSE_REQ_LEN + MMGSDI_GEN_SW_LEN)
        {
          sw_ptr[1] = 0x00;
          sw_ptr[3] = 0x00;
        }
        /* If pending Get Response data length is less than or equal to 0xFF,
           return implicit SW2 with the the pending length */
        else
        {
          sw_ptr[1] =  int32touint8(
                         mmgsdi_apdu_get_rsp_table_ptr[tbl_index]->apdu.data_len -
                         MMGSDI_GEN_SW_LEN);
          sw_ptr[3] = sw_ptr[1];
        }
      }
      /* All Get Response data has been fetched, return the final SW1/SW2 */
      else
      {
        mmgsdi_memscpy(sw_ptr,
          MMGSDI_GEN_SW_LEN,
          &mmgsdi_apdu_get_rsp_table_ptr[tbl_index]->apdu.data_ptr[apdu_len - MMGSDI_GEN_SW_LEN],
          MMGSDI_GEN_SW_LEN);
      }
    }
  }
  else
  {
    /* Invalid APDU Len */
    MMGSDIUTIL_DEBUG_MSG_ERROR_1("Invalid apdu len stored 0x%x ", apdu_len);

    status = MMGSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
     If all Get Response data has been fetched, or there is an error in
     handling the Get Response request, clear out cached item.
     Free out the memory location and wipe out the data associated with the
     index for the client's data
     ----------------------------------------------------------------------- */
  if((status == MMGSDI_ERROR) ||
     (mmgsdi_apdu_get_rsp_table_ptr[tbl_index]->apdu.data_len <= MMGSDI_GEN_SW_LEN))
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_apdu_get_rsp_table_ptr[tbl_index]->apdu.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_apdu_get_rsp_table_ptr[tbl_index]);
  }

  return status;
} /* mmgsdi_gen_get_rsp_table_retrieve */


/*===========================================================================
FUNCTION MMGSDI_GEN_RSP_TABLE_FLUSH_ALL

DESCRIPTION
  Function will clear all all entries in the GET APDU Get Response
  Cache Table.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Will Free Up any memory allocated by any of the Add Functions

===========================================================================*/
void  mmgsdi_gen_get_rsp_table_flush_all(
  mmgsdi_slot_id_enum_type  slot_id
)
{
  int32 index = 0;

  UIM_MSG_HIGH_1("Flushing out entire Get RSP APDU Cache for slot: 0x%x", slot_id);

  for (index=0; index < MMGSDI_MAX_AVAIL_GET_RSP_CACHE_ENTRIES; index++)
  {
    if(mmgsdi_apdu_get_rsp_table_ptr[index] == NULL )
    {
      continue;
    }
    if(mmgsdi_apdu_get_rsp_table_ptr[index]->slot_id == slot_id)
    {
      if (mmgsdi_apdu_get_rsp_table_ptr[index]->apdu.data_ptr != NULL)
      {
        /* Free up any allocated memory for any outstanding
        ** APDUs that resulted in a Get Response
        */
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
        mmgsdi_apdu_get_rsp_table_ptr[index]->apdu.data_ptr);
      }
      /* Free up allocated memory */
      MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_apdu_get_rsp_table_ptr[index]);
    }
  }
} /* mmgsdi_gen_get_rsp_table_flush_all */


/*===========================================================================
  FUNCTION:      MMGSDI_GEN_DO_GET_RESPONSE

  DESCRIPTION:
    This function will search the Get Response cache table for the
    response of the APDU just sent.

  DEPENDENCIES:
    None

  LIMITATIONS:
    TNone

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_gen_do_get_response(
  const mmgsdi_send_apdu_req_type * req_ptr
)
{
  uim_rpt_type                     * report_ptr      = NULL;
  mmgsdi_return_enum_type            mmgsdi_status   = MMGSDI_ERROR;
  mmgsdi_return_enum_type            get_rsp_status  = MMGSDI_ERROR;
  mmgsdi_data_type                   get_rsp;
  int32                              index           = 0;
  mmgsdi_client_req_extra_info_type* extra_param_ptr = NULL;
  int16                              get_resp_len    = 0;
  uint8                              channel_id      = 0;
  uint8                              sw[MMGSDI_GEN_SW_LEN];
  uint8                              get_rsp_data[MMGSDI_GEN_MAX_GET_RESPONSE_REQ_LEN + 1];

  memset(sw, 0x00, MMGSDI_GEN_SW_LEN);
  memset(get_rsp_data, 0x00, sizeof(get_rsp_data));
  memset(&get_rsp, 0x00, sizeof(mmgsdi_data_type));
  /* --------------------------------------------------------------------------
     Input validation
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL_2(req_ptr, req_ptr->data.data_ptr);

  UIM_MSG_HIGH_1("APDU length:0x%x for GET RESPONSE", req_ptr->data.data_len);

  /* Minimum mandated APDU length is 4, treat anything less as an error */
  if(req_ptr->data.data_len < MMGSDI_APDU_GET_RESPONSE_LEN_OFFSET)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }
  /* APDU length of 4 is valid, assume Le=0 in this case */
  else if(req_ptr->data.data_len == MMGSDI_APDU_GET_RESPONSE_LEN_OFFSET)
  {
    get_resp_len = 0;
  }
  /* When APDU length is greater than 4, retrieve Le from APDU */
  else
  {
    get_resp_len = req_ptr->data.data_ptr[MMGSDI_APDU_GET_RESPONSE_LEN_OFFSET];
  }

  /* --------------------------------------------------------------------------
     Retrieve the Channel ID from the CLA byte
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_channel_id_from_cla(req_ptr->data.data_ptr[0],
                                                      req_ptr->request_header.slot_id,
                                                      &channel_id);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("Could not retrieve Channel ID from CLA");
    return mmgsdi_status;
  }

  /* --------------------------------------------------------------------------
     Retrieve the Data from the Get Response Table
     ------------------------------------------------------------------------*/
  get_rsp.data_ptr = get_rsp_data;

  get_rsp_status = mmgsdi_gen_get_rsp_table_retrieve(
    req_ptr->request_header.client_id,
    req_ptr->request_header.slot_id,
    channel_id,
    get_resp_len,
    &get_rsp,
    sw);
  /* If Get Response data is not cached, return and send request down to UIMDRV. */
  if(get_rsp_status != MMGSDI_SUCCESS)
  {
    return get_rsp_status;
  }

  /*
   * Allocate and populate extra parameter.
   */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
                    req_ptr->request_header.request_type, (void*)req_ptr, &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* --------------------------------------------------------------------------
     Register the transaction with the Client Command Table maintained
     to align the response with the request
     ---------------------------------------------------------------------- */
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
        index,
        &req_ptr->request_header,
        NULL,
        extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return mmgsdi_status;
  }

  /* --------------------------------------------------------------------------
     Allocate Data for the UIM Response so that the Send APDU
     Response can be provided
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(report_ptr, sizeof(uim_rpt_type));
  if (report_ptr == NULL)
  {
    /* Not much we can do except use the static conf to indicate
    ** there has been a memory heap exhuastion error
    */
    mmgsdi_util_free_client_request_table_index(index);
    return MMGSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
     Proceed with populating the UIM Report Pointer to be provided to
     MMGSDI.  This must be a successful command.
     ------------------------------------------------------------------------*/
  report_ptr->cmd_transacted = TRUE;
  report_ptr->rpt_status     = UIM_PASS;
  report_ptr->rpt_type       = UIM_STREAM_ISO7816_APDU_R;
  report_ptr->user_data      = int32touint32(index);

  report_ptr->sw1            = (uim_sw1_type)sw[0];
  report_ptr->sw2            = (uim_sw2_type)sw[1];

  report_ptr->rpt.stream_iso7816_apdu.get_resp_sw1 = (uim_sw1_type)sw[2];
  report_ptr->rpt.stream_iso7816_apdu.get_resp_sw2 = (uim_sw2_type)sw[3];

  report_ptr->rpt.stream_iso7816_apdu.num_data_bytes = int32touint16(get_rsp.data_len);

  if(get_rsp.data_len > 0)
  {
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      report_ptr->rpt.stream_iso7816_apdu.data_ptr,
      report_ptr->rpt.stream_iso7816_apdu.num_data_bytes);

    if (report_ptr->rpt.stream_iso7816_apdu.data_ptr == NULL)
    {
      mmgsdi_util_free_client_request_table_index(index);
      MMGSDIUTIL_TMC_MEM_FREE(report_ptr);
      return MMGSDI_ERROR;
    }

    mmgsdi_memscpy(report_ptr->rpt.stream_iso7816_apdu.data_ptr,
                   report_ptr->rpt.stream_iso7816_apdu.num_data_bytes,
                   get_rsp.data_ptr,
                   int32touint32(get_rsp.data_len));
  }

  /* --------------------------------------------------------------------------
     Call the mmgsdi_uim_report function which is the callback for the UIM
     Task to call.  This is the way to get the MMGSDI Task to give itself
     a Response that would normally come back from the UIM Task.
     ----------------------------------------------------------------------- */
  mmgsdi_uim_report (report_ptr);

  /* --------------------------------------------------------------------------
     Clean up / free up any memory allocated in this function
     ----------------------------------------------------------------------- */
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(report_ptr->rpt.stream_iso7816_apdu.data_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(report_ptr);

  return MMGSDI_SUCCESS;
}/* mmgsdi_gen_do_get_response */


/*===========================================================================
  FUNCTION:      MMGSDI_GEN_PROCESS_SEND_APDU

  DESCRIPTION:
    This function will send an APDU to the UICC.

  DEPENDENCIES:
    None

  LIMITATIONS:
    This function is limited to the use of UICC ie. (technologies of type UICC only).

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_process_send_apdu (
  mmgsdi_send_apdu_req_type * req_ptr,
  mmgsdi_protocol_enum_type   protocol
)
{
  mmgsdi_return_enum_type    mmgsdi_status      = MMGSDI_SUCCESS;
  uint8                      i                  = 0;
  uint8                      channel_id         = 0;

  /* --------------------------------------------------------------------------
     Input validation
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  if(req_ptr->data.data_ptr == NULL ||
     req_ptr->data.data_len < MMGSDI_MIN_APDU_LEN)
  {
    return MMGSDI_ERROR;
  }

  if (req_ptr->data.data_ptr[1] == GET_RESPONSE /* For UICC && ICC */ )
  {
    /* Client is trying to issue a Get Response.
    ** Generate a Fake UIM Response Pointer. This command
    ** is no longer going to the UIM due to the Get Response
    ** Table which caches the Get response so that the clients
    ** doesn't need to. Upon completion of this function call,
    ** the Get Response data will be sent and queued for MMGSDI
    ** to process as if the UIM sent a response. Note: if the
    ** Get Response data is not present in cache, then proceed
    ** by passing the Get Response request to UIM.
    */
    if(mmgsdi_gen_do_get_response(req_ptr) == MMGSDI_SUCCESS)
    {
      return MMGSDI_SUCCESS;
    }

    UIM_MSG_HIGH_0("Fallback to send GET RESPONSE APDU down to UIM");
  }

   /* ---------------------------------------------------------------------------
     Get protocol
     NOTE THAT THIS DOES NOT CURRENTLY SUPPORT DUAL SLOTS
     ------------------------------------------------------------------------ */
  if(mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_ALLOW_SEND_APDU_ONLY, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) == MMGSDI_FEATURE_ENABLED
     && (req_ptr->request_header.orig_request_type == MMGSDI_SEND_APDU_REQ
         || req_ptr->request_header.orig_request_type == MMGSDI_SEND_APDU_EXT_REQ))
  {
    protocol = MMGSDI_UICC;
  }

  if (protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

  /*set channel state to READY_WITH_STREAMING*/
  mmgsdi_status = mmgsdi_util_get_channel_id_from_cla(
                    req_ptr->data.data_ptr[0],
                    req_ptr->request_header.slot_id,
                    &channel_id);

  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
    for (i = 0; i < MMGSDI_MAX_CHANNEL_INFO; i++)
    {
      if(mmgsdi_channel_info_ptr_table[i] != NULL &&
         mmgsdi_channel_info_ptr_table[i]->channel_id == channel_id &&
         mmgsdi_channel_info_ptr_table[i]->slot_id == req_ptr->request_header.slot_id &&
         mmgsdi_channel_info_ptr_table[i]->channel_state == MMGSDI_CHANNEL_STATE_READY)
      {
        mmgsdi_channel_info_ptr_table[i]->channel_state = MMGSDI_CHANNEL_STATE_APDU_STREAMING_DONE;
        break;
      }
    }
  }

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  if(protocol == MMGSDI_UICC)
  {
    mmgsdi_status = mmgsdi_uim_uicc_send_apdu(req_ptr);
  }
  else if (protocol == MMGSDI_ICC)
  {
    mmgsdi_status = mmgsdi_uim_icc_send_apdu(req_ptr);
  }
  else
  {
    /* Currently not supported */
    mmgsdi_status = MMGSDI_NOT_SUPPORTED;
  }
  return mmgsdi_status;
}/* mmgsdi_gen_send_apdu */


/*===========================================================================
  FUNCTION:      MMGSDI_GEN_SESSION_GET_INFO

  DESCRIPTION:
    This function will handle session get info request.

  DEPENDENCIES:
    NONE

  LIMITATIONS:
    NONE

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
  SIDE EFFECTS:
    NONE

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_session_get_info(
  const mmgsdi_session_get_info_req_type       *req_ptr
)
{
  mmgsdi_return_enum_type                 mmgsdi_status            = MMGSDI_INCORRECT_PARAMS;
  int32                                   index                    = 0;
  mmgsdi_client_req_extra_info_type      *extra_param_ptr          = NULL;
  mmgsdi_session_info_type               *session_info_ptr         = NULL;
  mmgsdi_channel_info_type               *channel_info_ptr         = NULL;
  mmgsdi_int_app_info_type               *app_info_ptr             = NULL;
  mmgsdi_session_id_type                  mmgsdi_session_id        = 0;
  mmgsdi_session_get_info_extra_info_type session_info_extra_param;

  /* --------------------------------------------------------------------------
     Validate the Input Parameters checks.
     ----------------------------------------------------------------------- */
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  memset(&session_info_extra_param,
         0x00,
         sizeof(mmgsdi_session_get_info_extra_info_type));

  /* get the session info */
  if(req_ptr->session_info_query.query_type == MMGSDI_SESSION_INFO_QUERY_BY_ID)
  {
    mmgsdi_session_id = req_ptr->session_info_query.query_key.session_id;
  }
  else if(req_ptr->session_info_query.query_type ==
                                        MMGSDI_SESSION_INFO_QUERY_BY_TYPE)
  {
    if (MMGSDI_SUCCESS != mmgsdi_util_get_mmgsdi_session_id_from_provisioning_type(
                            req_ptr->session_info_query.query_key.session_type,
                            &mmgsdi_session_id))
    {
       UIM_MSG_ERR_1("Invalid Session type requested=0x%x ",
                     req_ptr->session_info_query.query_key.session_type);
       return MMGSDI_INCORRECT_PARAMS;
    }
  }
  else
  {
    UIM_MSG_ERR_1("Invalid Session Query type=0x%x ",
                  req_ptr->session_info_query.query_type);
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* Get app info corresponding to the Session ID */
  mmgsdi_status = mmgsdi_util_get_session_app_info(
                    mmgsdi_session_id, NULL, NULL, &app_info_ptr,
                    &channel_info_ptr,&session_info_ptr, NULL);

  if(mmgsdi_status != MMGSDI_SUCCESS ||
     session_info_ptr == NULL ||
     channel_info_ptr == NULL ||
     app_info_ptr == NULL)
  {
    UIM_MSG_ERR_0("Session Info unavailable");
    return MMGSDI_ERROR;
  }
  session_info_extra_param.slot_id = channel_info_ptr->slot_id;
  session_info_extra_param.channel_id = channel_info_ptr->channel_id;
  session_info_extra_param.session_type = session_info_ptr->session_type;
  mmgsdi_memscpy(&session_info_extra_param.app_data,
                 sizeof(session_info_extra_param.app_data),
                 &app_info_ptr->app_data,
                 sizeof(mmgsdi_aid_type));
  session_info_extra_param.app_state = app_info_ptr->app_state;

  /* --------------------------------------------------------------------------
     Get an index from the table used to store client information.
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
                    MMGSDI_SESSION_GET_INFO_REQ,
                    (void*)&session_info_extra_param,
                    &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Unable to populate extra param 0x%x", mmgsdi_status);
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }/* end of if (mmgsdi_status != MMGSDI_SUCCESS) */

  /* --------------------------------------------------------------------------
     Store the Client Information for later use when the Response is provided
     by the UIM
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                                              index,
                                              &req_ptr->request_header,
                                              NULL,
                                              extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return mmgsdi_status;
  }

  /* send out the response */
  mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(
                    index, req_ptr->request_header.request_type,
                    MMGSDI_SUCCESS);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }
  return MMGSDI_SUCCESS;
}/* mmgsdi_gen_session_get_info */


/*===========================================================================
  FUNCTION:      MMGSDI_GEN_SESSION_READ_PRL

  DESCRIPTION:
    This function will handle session read prl request.

  DEPENDENCIES:
    NONE

  LIMITATIONS:
    NONE

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
  SIDE EFFECTS:
    NONE

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_session_read_prl(
  const mmgsdi_session_read_prl_req_type       *req_ptr
)
{
  mmgsdi_return_enum_type                 mmgsdi_status     = MMGSDI_INCORRECT_PARAMS;
  int32                                   index             = 0;
  mmgsdi_client_req_extra_info_type      *extra_param_ptr   = NULL;
  mmgsdi_session_info_type               *session_info_ptr  = NULL;
  mmgsdi_session_read_prl_extra_info_type session_read_prl_extra_param;
  mmgsdi_int_app_info_type               *int_app_info_ptr  = NULL;
  uint8                                   prl_index         = 0;
  mmgsdi_technology_enum_type             dummy_tech        = MMGSDI_TECH_UNKNOWN;
  mmgsdi_attribute_enum_type              attribute         = MMGSDI_ATTR_UNKNOWN;

  /* --------------------------------------------------------------------------
     Validate the Input Parameters checks.
     ----------------------------------------------------------------------- */
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                          req_ptr->request_header.session_id,NULL,NULL,
                          &int_app_info_ptr,
                          NULL,&session_info_ptr,NULL);

  if(mmgsdi_status != MMGSDI_SUCCESS ||
     session_info_ptr == NULL ||
     int_app_info_ptr == NULL)
  {
    UIM_MSG_ERR_1("Invalid session ID = 0x%x",
                  req_ptr->request_header.session_id);
    return MMGSDI_INCORRECT_PARAMS;
  }

  if((!mmgsdi_util_is_1x_prov_session(session_info_ptr->session_type)) || 
     ((int_app_info_ptr->app_data.app_type != MMGSDI_APP_CSIM ||
       int_app_info_ptr->app_data.app_type != MMGSDI_APP_RUIM) &&
      (int_app_info_ptr->app_state != MMGSDI_APP_STATE_READY)))
  {
    UIM_MSG_ERR_3("app type: 0x%x app state: 0x%x session_type: 0x%x",
                  int_app_info_ptr->app_data.app_type,
                  int_app_info_ptr->app_state,
                  session_info_ptr->session_type);
    return MMGSDI_INCORRECT_PARAMS;
  }

  (void)mmgsdi_util_get_tech_and_attr_from_session_type(
                                         session_info_ptr->session_type,
                                         &dummy_tech,
                                         &attribute);
  prl_index = (uint8)attribute;
  if (prl_index > MMGSDI_TER_PRL_INDEX)
  {
    return MMGSDI_ERROR;
  }

#ifdef FEATURE_MMGSDI_3GPP2
  /* If during initialization time (post pin and SUB_READY) PRL was not
     validated because SD was not init then do the PRL validation now.
     In case it is not success, no need to return ERROR from here. We
     will copy all 0s and send to CM in cnf handling as legacy behavior */
  if (mmgsdi_1x_prl[prl_index].prl_validated == FALSE)
  {
    (void)mmgsdi_card_init_1x_read_prl_init(session_info_ptr->session_id);
  }
#endif /* FEATURE_MMGSDI_3GPP2 */

  session_read_prl_extra_param.prl_ptr = mmgsdi_1x_prl[prl_index].prl_data_ptr;

  /* --------------------------------------------------------------------------
     Get an index from the table used to store client information.
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
                    MMGSDI_SESSION_READ_PRL_REQ,
                    (void*)&session_read_prl_extra_param,
                    &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Unable to populate extra param 0x%x", mmgsdi_status);
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }/* end of if (mmgsdi_status != MMGSDI_SUCCESS) */

  /* --------------------------------------------------------------------------
     Store the Client Information for later use when the Response is provided
     by the UIM
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                                              index,
                                              &req_ptr->request_header,
                                              NULL,
                                              extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    return mmgsdi_status;
  }

  /* send out the response */
  mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(
                    index, req_ptr->request_header.request_type,
                    MMGSDI_SUCCESS);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_gen_session_read_prl */


/*===========================================================================
  FUNCTION:      MMGSDI_GEN_PROV_APP_INIT_COMPLETE

  DESCRIPTION:
    This function will handle prov app init complete request

  DEPENDENCIES:
    NONE

  LIMITATIONS:
    NONE

  RETURN VALUE:
    MMGSDI_SUCCESS:        The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
  SIDE EFFECTS:
    NONE

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_prov_app_init_complete(
  const mmgsdi_session_prov_app_init_complete_req_type  *req_ptr
)
{
  mmgsdi_return_enum_type                 mmgsdi_status        = MMGSDI_SUCCESS;
  int32                                   index                = 0;
  mmgsdi_session_info_type               *session_info_ptr     = NULL;
  int32                                   client_index         = 0;
  int32                                   session_index        = 0;
  mmgsdi_slot_id_enum_type                slot_id              = MMGSDI_MAX_SLOT_ID_ENUM;
  uint8                                   slot_index           = MMGSDI_SLOT_1_INDEX;

  /* --------------------------------------------------------------------------
     Validate the Input Parameters checks.
     ----------------------------------------------------------------------- */
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  mmgsdi_status = mmgsdi_util_get_client_and_session_index(
                    req_ptr->request_header.session_id,
                    &client_index,
                    &session_index);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Invalid session id 0x%x", req_ptr->request_header.session_id);
    return MMGSDI_INCORRECT_PARAMS;
  }
  /* mmgsdi_util_get_client_and_session_index success guarantees valid dereferences
     into mmgsdi_client_id_reg_table and session_info_ptr */
  session_info_ptr =
    mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index];

  if(session_info_ptr == NULL)
  {
    UIM_MSG_ERR_1("Null session_ptr for session_id 0x%x",
                  (req_ptr->request_header.session_id & 0xFFFFFFFF));
    return MMGSDI_ERROR;
  }

  if(!session_info_ptr->notify_init)
  {
    UIM_MSG_ERR_0("App init complete not expected for this session");
    return MMGSDI_ERROR;
  }

  session_info_ptr->init_complete = req_ptr->init_status;

  mmgsdi_status = mmgsdi_util_get_session_app_info(req_ptr->request_header.session_id,
                                                   &slot_id, NULL, NULL, NULL,
                                                   NULL, NULL);
  if((mmgsdi_status != MMGSDI_SUCCESS) ||
     (slot_id == MMGSDI_MAX_SLOT_ID_ENUM))
  {
    UIM_MSG_ERR_0("Cannot retrieve slot id");
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_get_slot_index(slot_id, &slot_index);
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
    mmgsdi_status = mmgsdi_session_refresh_complete(
                      req_ptr->request_header.session_id,
                      req_ptr->init_status,
                      mmgsdi_util_internal_cb,
                      0);

    if (MMGSDI_SUCCESS != mmgsdi_status)
    {
      UIM_MSG_ERR_1("Unable to send Refresh Complete 0x%x", mmgsdi_status);
    }
  } /* if(mmgsdi_refresh_info_ptr[slot_index] != NULL) */

  /* --------------------------------------------------------------------------
     Get an index from the table used to store client information.
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* --------------------------------------------------------------------------
     Store the Client Information for later use when the Response is provided
     by the UIM
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                                              index,
                                              &req_ptr->request_header,
                                              NULL,
                                              NULL);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }

  /* send out the response */
  mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(
                    index, req_ptr->request_header.request_type,
                    MMGSDI_SUCCESS);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }
  return MMGSDI_SUCCESS;
}/* mmgsdi_gen_prov_app_init_complete */


/*===========================================================================
  FUNCTION:      MMGSDI_GEN_SESSION_MANAGE_ILLEGAL_SUBSCRIPTION

  DESCRIPTION:
    This function will handle a manage subscription request which is triggered
    when a client wants to mark an application illegal based on network
    rejection or later mark it back as legal.

  DEPENDENCIES:
    NONE

  LIMITATIONS:
    NONE

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
  SIDE EFFECTS:
    Makes application connected with session as illegal or ready, sends illegal
    subscription event or legal subscription event to all sessions associated
    with that app

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_session_manage_illegal_subscription(
  const mmgsdi_session_manage_illegal_subscription_req_type  *req_ptr
)
{
  mmgsdi_channel_info_type            *channel_info_ptr  = NULL;
  mmgsdi_evt_session_notify_type       notify_type;
  mmgsdi_return_enum_type              mmgsdi_status = MMGSDI_SUCCESS;

  memset(&notify_type, 0x00, sizeof(mmgsdi_evt_session_notify_type));

  if(req_ptr == NULL)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_get_session_and_channel_info(
                    req_ptr->request_header.session_id,
                    NULL, &channel_info_ptr);

  if((mmgsdi_status != MMGSDI_SUCCESS) ||
     (channel_info_ptr == NULL) ||
     (channel_info_ptr->app_info_ptr == NULL))
  {
    return MMGSDI_ERROR;
  }

  /* Build the appropriate notification */
  notify_type.notify_type = MMGSDI_EVT_NOTIFY_ALL_SESSIONS;
  notify_type.slot_id     = channel_info_ptr->slot_id;
  notify_type.session_id  = 0;

  switch(req_ptr->legal_status)
  {
    /* Mark this application as legal */
    case MMGSDI_SESSION_APP_IS_LEGAL:
      if (channel_info_ptr->app_info_ptr->app_state != MMGSDI_APP_STATE_ILLEGAL)
      {
        UIM_MSG_ERR_1("Manage illegal subscription - Invalid app state: 0x%x",
                      channel_info_ptr->app_info_ptr->app_state);
        return MMGSDI_ERROR;
      }

      channel_info_ptr->app_info_ptr->app_state =
        MMGSDI_APP_STATE_READY;

      UIM_MSG_HIGH_1("Application for session %d is MARKED AS LEGAL BY REQUEST",
                     req_ptr->request_header.session_id);

      mmgsdi_evt_build_and_send_legal_session(
        notify_type,
        (const mmgsdi_int_app_info_type*) channel_info_ptr->app_info_ptr);
      break;

    /* Mark this Application as illegal */
    case MMGSDI_SESSION_APP_IS_ILLEGAL:
      channel_info_ptr->app_info_ptr->app_state =
        MMGSDI_APP_STATE_ILLEGAL;

      UIM_MSG_HIGH_1("Application for session %d is MARKED AS ILLEGAL BY REQUEST",
                     req_ptr->request_header.session_id);

      if(channel_info_ptr->app_info_ptr->service_status != SIMLOCK_SRV_STATUS_EXPIRED)
      {
        channel_info_ptr->app_info_ptr->service_status = SIMLOCK_SRV_STATUS_EXPIRED;
#ifdef FEATURE_SIMLOCK
        /* If the device is a single slot device, no need to
           run the algo in case of illegal event as the network
           policies apply to multi-slot devices */
        if((mmgsdi_generic_data_ptr != NULL) &&
           (mmgsdi_generic_data_ptr->num_slots > 1))
        {
          mmgsdi_simlock_build_and_run_algo(FALSE);
        }
#endif /* FEATURE_SIMLOCK */
      }

      mmgsdi_evt_build_and_send_illegal_session(
        notify_type,
        (const mmgsdi_int_app_info_type*) channel_info_ptr->app_info_ptr);
      break;

    default:
      UIM_MSG_ERR_0("Invalid manage illegal subscription request attempted.");
      return MMGSDI_ERROR;
  }

  return mmgsdi_status;
}/* mmgsdi_gen_session_manage_illegal_subscription */


/*===========================================================================
  FUNCTION:      MMGSDI_GEN_AVAILABLE_APPS_REQ

  DESCRIPTION:
    This function will handle get available apps request

  DEPENDENCIES:
    NONE

  LIMITATIONS:
    NONE

  RETURN VALUE:
    MMGSDI_SUCCESS:        The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
  SIDE EFFECTS:
    NONE

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_available_apps_req(
  const mmgsdi_available_apps_req_type * req_ptr
)
{
  mmgsdi_return_enum_type                 mmgsdi_status        = MMGSDI_SUCCESS;
  int32                                   index                = 0;

  /* --------------------------------------------------------------------------
     Validate the Input Parameters checks.
     ----------------------------------------------------------------------- */
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  /* --------------------------------------------------------------------------
     Get an index from the table used to store client information.
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* --------------------------------------------------------------------------
     Store the Client Information for later use when the Response is provided
     by the UIM
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                                              index,
                                              &req_ptr->request_header,
                                              NULL,
                                              NULL);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }

  /* send out the response */
  mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(
                    index, req_ptr->request_header.request_type,
                    MMGSDI_SUCCESS);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }
  return MMGSDI_SUCCESS;
}/* mmgsdi_gen_available_apps_req */


/*===========================================================================
  FUNCTION:      MMGSDI_GEN_GET_CPHS_INFO

  DESCRIPTION:
    This function will handle get cphs information request

  DEPENDENCIES:
    NONE

  LIMITATIONS:
    NONE

  RETURN VALUE:
    MMGSDI_SUCCESS:        The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
  SIDE EFFECTS:
    NONE

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_get_cphs_info(
  const mmgsdi_session_get_cphs_info_req_type *req_ptr
)
{
  mmgsdi_return_enum_type                 mmgsdi_status = MMGSDI_SUCCESS;
  int32                                   index         = 0;
  mmgsdi_session_type_enum_type           session_type  = MMGSDI_MAX_SESSION_TYPE_ENUM;

  /* --------------------------------------------------------------------------
     Validate the Input Parameters checks.
     ----------------------------------------------------------------------- */
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  /* --------------------------------------------------------------------------
     Validate input Session type: has to be a GW provisioning type
     ----------------------------------------------------------------------- */
  mmgsdi_status = mmgsdi_util_get_session_type(req_ptr->request_header.session_id,
                                               &session_type,
                                               NULL);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Could not retrieve Session Type for Session ID: 0x%x",
                  req_ptr->request_header.session_id);
    return MMGSDI_INCORRECT_PARAMS;
  }

  switch (session_type)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
    case MMGSDI_GW_PROV_SEC_SESSION:
    case MMGSDI_GW_PROV_TER_SESSION:
      break;

    default:
      return MMGSDI_INCORRECT_PARAMS;
  }

  /* --------------------------------------------------------------------------
     Get an index from the table used to store client information.
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* --------------------------------------------------------------------------
     Store the Client Information for later use when the Response is provided
     by the UIM
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                                              index,
                                              &req_ptr->request_header,
                                              NULL,
                                              NULL);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }

  /* send out the response */
  mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(
                    index, req_ptr->request_header.request_type,
                    MMGSDI_SUCCESS);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }
  return MMGSDI_SUCCESS;
}/* mmgsdi_gen_get_cphs_info */


/*===========================================================================
  FUNCTION:      MMGSDI_GEN_GET_APP_CAPABILITIES

  DESCRIPTION:
    This function will handle get app capability request

  DEPENDENCIES:
    NONE

  LIMITATIONS:
    NONE

  RETURN VALUE:
    MMGSDI_SUCCESS:        The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
  SIDE EFFECTS:
    NONE

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_get_app_capabilities(
  const mmgsdi_session_get_app_capabilities_req_type *req_ptr
)
{
  mmgsdi_return_enum_type                 mmgsdi_status        = MMGSDI_SUCCESS;
  int32                                   index                = 0;

  UIM_MSG_MED_0("MMGSDI GEN GET APP CAPABILITIES CMD");

  /* --------------------------------------------------------------------------
     Validate the Input Parameters checks.
     ----------------------------------------------------------------------- */
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  /* --------------------------------------------------------------------------
     Get an index from the table used to store client information.
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* --------------------------------------------------------------------------
     Store the Client Information for later use when the Response is provided
     by the UIM
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                                              index,
                                              &req_ptr->request_header,
                                              NULL,
                                              NULL);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }

  /* send out the response */
  mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(
                    index, req_ptr->request_header.request_type,
                    MMGSDI_SUCCESS);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }
  return MMGSDI_SUCCESS;
}/* mmgsdi_gen_get_app_capabilities */


/*===========================================================================

FUNCTION mmgsdi_gen_onchip_auto_activate

DESCRIPTION
    This function auto activates the OnChip USIM/SIM feature for the given
    slot. This feature is controlled by an EFS item. If the EFS item indicates
    that OnChip feature is to be auto-activated, the function queues the
    command to MMGSDI for activating it for SIM or USIM (which is also
    indicated in the EFS item). The configuration required for activating
    OnChip is read from an EFS item. If the OnChip auto-activation has been
    successfully kicked off, the function returns SUCCESS.

DEPENDENCIES
    None

RETURN VALUE
    MMGSDI_SUCCESS: If OnChip activation req has been successfully queued
    MMGSDI_ERROR:   Otherwise

SIDE EFFECTS
    None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_onchip_auto_activate(
  mmgsdi_slot_id_enum_type req_slot_id
)
{
  mmgsdi_return_enum_type             mmgsdi_status               = MMGSDI_ERROR;
  mmgsdi_slot_data_type              *slot_data_ptr               = NULL;
  int32                               efs_onchip_config_file_size = 0;
  mmgsdi_onchip_sim_config_data_type  mmgsdi_onchip_data;
  uint8                               mmgsdi_onchip_data_buf[MMGSDI_ACTIVATE_ONCHIP_SIM_CONFIG_MAX_SIZE];
  uint8                               req_slot_index              = MMGSDI_SLOT_1_INDEX;
  char                               *file_path_ptr               = NULL;

  memset(mmgsdi_onchip_data_buf, 0x00, sizeof(mmgsdi_onchip_data_buf));
  mmgsdi_onchip_data.data_ptr = mmgsdi_onchip_data_buf;

  if(MMGSDI_SUCCESS != mmgsdi_util_get_slot_index(req_slot_id, &req_slot_index))
  {
    return MMGSDI_ERROR;
  }

  /* Check the auto-onchip-activation global to see if Onchip should be auto
     activated or not */

  if(mmgsdi_generic_data_ptr == NULL ||
     mmgsdi_generic_data_ptr->onchip_act_data[req_slot_index].auto_activation_enabled == FALSE)
  {
    UIM_MSG_ERR_1("Auto onchip activation disabled for slot 0x%x", req_slot_id);
    return MMGSDI_ERROR;
  }

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(req_slot_id);

  /* If Onchip is already activated, do not re-activate it and return SUCCESS */
  if(slot_data_ptr != NULL &&
     slot_data_ptr->onchip_sim_data.state == MMGSDI_ONCHIP_SIM_INIT)
  {
    UIM_MSG_HIGH_1("Onchip already activated for slot 0x%x", req_slot_id);
    return MMGSDI_SUCCESS;
  }

  /* Read the onchip config file and continue with activating the OnChip
     support with the data in that file */

  /* Get the OnChip config file data for slot specific simulation */
  switch (req_slot_id)
  {
    case MMGSDI_SLOT_1:
      file_path_ptr = UIM_COMMON_EFS_PATH_MMGSDI_ONCHIP_CONFIG_EFS_FILE_SLOT1;
      break;
    case MMGSDI_SLOT_2:
      file_path_ptr = UIM_COMMON_EFS_PATH_MMGSDI_ONCHIP_CONFIG_EFS_FILE_SLOT2;
      break;
    case MMGSDI_SLOT_3:
      file_path_ptr = UIM_COMMON_EFS_PATH_MMGSDI_ONCHIP_CONFIG_EFS_FILE_SLOT3;
      break;
    default:
     return MMGSDI_ERROR;
  }

  if(MMGSDI_SUCCESS !=
         gsdi_fs_get_file_size(file_path_ptr,(int32*)&efs_onchip_config_file_size,
                               UIM_COMMON_EFS_DEVICE))
  {
    return MMGSDI_ERROR;
  }

  if(efs_onchip_config_file_size < MMGSDI_ONCHIP_TLV_PAYLOAD ||
     efs_onchip_config_file_size > MMGSDI_ACTIVATE_ONCHIP_SIM_CONFIG_MAX_SIZE)
  {
    UIM_MSG_ERR_1("Invalid data in OnChip config file for slot 0x%x",
                  req_slot_id);
    return MMGSDI_ERROR;
  }

  /* Read the OnChip config file for onchip activation */
  if(MMGSDI_SUCCESS !=
         gsdi_efs_read_file_with_len(file_path_ptr, efs_onchip_config_file_size,
                                     mmgsdi_onchip_data.data_ptr, UIM_COMMON_EFS_DEVICE))
  {
    return MMGSDI_ERROR;
  }

  /* Lets construct the length of onchip data to be fed into the MMGSDI API.
     The Onchip data present in the global(read from EFS item) starts with a
     MASTER_TAG (1 byte), followed by 2 bytes for data_length followed by the
     actual data. So, lets calculate the total data length:
     1. Get the number of bytes stored in second and third byte of the onchip
        data buf (reserved for length of onchip data)
     2. Increment it by the OVERHEAD length (1 byte for MASTER_TAG and two
        bytes for length of onchip data */
  mmgsdi_onchip_data.data_len = mmgsdi_onchip_data.data_ptr[1];
  mmgsdi_onchip_data.data_len += mmgsdi_onchip_data.data_ptr[2] * 0x100;
  mmgsdi_onchip_data.data_len += MMGSDI_ONCHIP_TLV_PAYLOAD;

  /* In order to avoid buffer overflow, make sure the data length does not
     exceed the data buffer size */
  if(mmgsdi_onchip_data.data_len > sizeof(mmgsdi_onchip_data_buf))
  {
    UIM_MSG_ERR_0("Invalid Onchip activation data length");
    return MMGSDI_ERROR;
  }

  switch(mmgsdi_generic_data_ptr->onchip_act_data[req_slot_index].app_type)
  {
    case MMGSDI_APP_SIM:
      /* start OnChipSIM command processing for SIM */
      mmgsdi_status = mmgsdi_activate_onchip_sim(
               mmgsdi_generic_data_ptr->client_id, /* MMGSDI client ID */
               req_slot_id,
               mmgsdi_onchip_data,
               mmgsdi_util_internal_cb,
               0x00);
      break;
    case MMGSDI_APP_USIM:
      /* start OnChipSIM command processing for USIM */
      mmgsdi_status = mmgsdi_activate_onchip_usim(
               mmgsdi_generic_data_ptr->client_id, /* MMGSDI client ID */
               req_slot_id,
               mmgsdi_onchip_data,
               mmgsdi_util_internal_cb,
               0x00);
      break;
    default:
      mmgsdi_status = MMGSDI_ERROR;
      break;
  }

  return mmgsdi_status;
} /* mmgsdi_gen_onchip_auto_activate */

