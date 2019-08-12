/*===========================================================================

===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2009 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdiag/src/uimdiag_mmgsdi.c#1 $$ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who   what, where, why
--------   ---   -----------------------------------------------------------
05/22/16   vdc   Remove F3 messages for memory allocation failure
11/05/15   ks    Rel12 polling enhancement
09/22/15   lm    Fix KW errors
04/11/15   ar    Correcting the source length provided in memscpy()
09/17/14   vr    Correction in heap variable created in stack reduction
09/09/14   vr    Reduce stack usage
07/31/14   hh    Removed FEATURE_MMGSDIDIAG_TEST_AUTOMATION
07/10/14   tl    Remove ALWAYS OFF feature FEATURE_MMGSDI_PKCS15
04/22/14   at    Critical KW fixes
01/27/14   df    Featurise file with FEATURE_UIMDIAG
01/15/14   ar    Parsing file path in session open and send card status API cnf
01/10/14   df    Use v2.0 diag macros
12/21/13   spo   Fix for opening a non-prov session on slot 3
11/05/13   df    Removed unnecessary explicit type casting, add NULL ptr check
10/08/13   df    Correct handling default case in uimdiag_mmgsdi_event_cb()
09/26/13   ar    Full path to be copied while using memscpy.
09/25/13   bcho  Query type parsed in UIMDIAG_MMGSDI_SESSION_GET_INFO_CMD
09/24/13   bcho  Parsing for SPN disp bit, RPLMN disp bit and roaming status
09/19/13   vv    Fix for a possible buffer overflow
09/16/13   yt    Remove redundant code
05/24/13   bcho  Proper parsing of get feature data in UIMDIAG_MMGSDI_PERSO_CNF
05/17/13   hn    Support for Alpha for Refresh
05/06/13   sw    Changes made for secure version of memcpy
04/24/13   rr    Featurized optional logs
04/04/13   abd   Added utility function to update app information
04/16/13   bcho  Respond to script even if response handling fails
04/12/13   spo   Added code to handle dynamic data in confirmation
03/21/13   bcho  Heap memory corruption resolved
03/19/13   vdc   Fixed MMGSDI's status in immediate response for UIMDIAG cmd
03/14/13   bcho  Replace local variable with heap buffer to reduce stack usage
03/13/13   vdc   Fixing invalid access due to non-memset of API Parameter
12/12/12   at    Fixed critical KW warnings
11/23/12   bcho  Support for RAT type param in get_operator_name API
10/03/12   tl    Fix Perso CNF crash
10/01/12   tl    Added support for dynamically allocated response data for cnf
09/26/12   tl    Added support for unblock dck retries
09/03/12   bcho  Added support for MMGSDI_EONS_OPL_PNN_CACHE_READY_EVT
08/31/12   abg   Updated UPIN replacement in Get All Pin Status response
02/29/12   bcho  Q6 compilation warnings removed
12/27/11   kk    Fixed sync command handling
12/15/11   kk    Fixed compilation warnings
11/23/11   adp   MMGSDI status should be returned correctly in erroroneous
                 conditions too
11/21/11   bcho  Incorporated review comments
11/21/11   kk    Fixed get all available apps async rsp handling
11/17/11   kk    Added get all available apps async support
11/10/11   bcho  Support for Get Operator Name API modified
10/24/11   adp   Removing Num_of_files from request field for naa_refresh
10/24/11   adp   Removing support for UIMDIAG_MMGSDI_SAP_PIN_VERIFY_CMD,
                 UIMDIAG_MMGSDI_SAP_INITIALIZE_CMD and
                 UIMDIAG_MMGSDI_SAP_DEREGISTER_CMD
10/17/11   adp   Fixing KW errors
10/18/11   bcho  Adding support for mmgsdi_session_open_ext API
10/14/11   bcho  Added support for Get Operator Name API
10/10/11   nb    Fix for compilation error
10/10/11   nb    Support for SAP CONNECT/SAP DISCONNECT
10/08/11   adp   Changing uimdiag_mmgsdi_rsp_session_get_info_pyld_type and
                 uimdiag_mmgsdi_session_read_cache_rsp_type to match ICD.
10/03/11   adp   Fixing compiler warning
09/26/11   adp   Fixed crash, where memory pointer is NULL, but was being
                 assigned a value
09/15/11   adp   Adding support fo trigger Refresh
09/15/11   nb    Support for new refresh stage
08/23/11   adp   Support for handling DF_ENUM access confirmation
08/24/11   nk    Fix buffer overflow KW error
08/22/11   av    Added support for pdown,pup,activate onchip,send apdu commands
08/18/11   adp   Filling the pin operation field in response for
                 uimdiag_mmgsdi_proc_pin_operation_cnf
08/05/11   av    UIMDiag now doesn't return error if search pattern is null;
                 It's handled in mmgsdi now.
06/09/11   adp   Fixing get file attr by path access, Fixing search and seek
03/14/11   adp   Initializing the rsp pointer payload to zero, in order to
                 handle error conditions
02/22/11   ea    Fix compiler warnings
02/02/10   ms    Fixed compiler warnings
12/14/10   adp   PIN/PERSO commands when tried with invalid key, should
                 return num of retries
12/03/10   adp   Adding support for a number of commands
11/15/10   adp   Displaying MMGSDI Status (delayed) after the MMGSDI API call
11/09/10   adp   DIAG Header status to be always set to SUCCESS, so that when
                 processing commands with wrong parameters UIMDIAG will not
                 indicate "error".
11/09/10   adp   Adding Event support in UIMDIAG
10/05/10   adp   Fixing Lint, KW  warnings
09/07/10   adp   Adding Support for dck_num_retries,lock_down_enable_ltd_access,
                 seek and search commands
08/09/10   adp   Adding support for service enable, disable, get perso key,
                 is service available, perso unblock.
06/09/10   adp   Fixing Compiler Warnings
05/26/10   yt    Fixed compiler warnings
04/27/10   shr   Added Hiddenkey support
03/25/10   adp   Adding support for mmgsdi_session_deactivate_provisioning,
                 mmgsdi_session_get_app_capabilities,
                 mmgsdi_session_get_cphs_info,
                 isim_authenticate, send_card_status,event handling,
                 create_pkcs15_lookup_table
03/24/10   adp   Fixing compiler warnings
03/19/10   vs    Fixed AID and label length check
03/19/10   rk    Added support for JCDMA get info command and event handling
03/24/10   adp   Fixing compiler warnings
02/22/10   adp   Adding support for Perso Commands
02/03/10   adp   Fix for memory copy issue while writing to record
12/27/09   ap    Adding support for Refresh commands
01/07/10   adp   KlocWork Errors
12/17/09   rk    Fix for uimdiag small heap memory alloction issue
12/03/09   rk    Adding support for get file attributes, read and write commands
10/19/09   adp   Adding pin commands
10/18/09   adp   Restructing the code to make it more readable.
                 Adding support for Activate_or_switch_prov, session_get_info
09/02/09   adp   Initial Revision -Added initial mmgsdi session based commands


=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "uim_variation.h"
#ifdef FEATURE_UIMDIAG
#include "mmgsdisessionlib.h"
#include "mmgsdisessionlib_v.h"
#include "uimdiag_util.h"
#include "mmgsdilib.h"
#include "mmgsdilib_v.h"
#include "comdef.h"
#include "uimdiag.h"
#include "uimdiag_mmgsdi.h"
#include "uimdiag_mmgsdi_util.h"
#include "mmgsdilib_common.h"
#include "intconv.h"
#include "diagpkt.h"
#include "diagcmd.h"
#include "event.h" /* event_report function */
#include "rex.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define UIMDIAG_IMEI_KEY_SEPARATOR     0x3A

/*=============================================================================

                              FUNCTIONS

=============================================================================*/

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_PROC_CLIENT_ID_AND_EVT_REG_CNF

DESCRIPTION
  Function to populate the response pointer with the confirmation data

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_proc_client_id_and_evt_reg_cnf(
  uimdiag_generic_rsp_type      *rsp_ptr,
  const mmgsdi_cnf_type         *cnf_data_ptr
)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cnf_data_ptr, rsp_ptr);

  (void)memset((void*)&rsp_ptr->rsp_payload.response.client_id_and_evt_reg,
               0x00,sizeof(uimdiag_mmgsdi_rsp_client_id_and_evt_reg_pyld_type));

  rsp_ptr->rsp_payload.response.client_id_and_evt_reg.client_id_low =
    UIMDIAG_LODWORD(
      cnf_data_ptr->client_id_and_evt_reg_cnf.response_header.client_id);

  rsp_ptr->rsp_payload.response.client_id_and_evt_reg.client_id_high =
    UIMDIAG_HIDWORD(
      cnf_data_ptr->client_id_and_evt_reg_cnf.response_header.client_id);

  return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_proc_client_id_and_evt_reg_cnf */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_PROC_CLIENT_ID_AND_EVT_DEREG_CNF

DESCRIPTION
  Function to populate the response pointer with the confirmation data
  for client id and evt dereg

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_proc_client_id_and_evt_dereg_cnf(
  uimdiag_generic_rsp_type      *rsp_ptr,
  const mmgsdi_cnf_type         *cnf_data_ptr
)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cnf_data_ptr, rsp_ptr);

  (void)memset((void*)&rsp_ptr->rsp_payload.response.client_id_and_evt_dereg,
               0x00,sizeof(uimdiag_mmgsdi_rsp_client_id_and_evt_dereg_pyld_type));

  rsp_ptr->rsp_payload.response.client_id_and_evt_dereg.client_id_low =
    UIMDIAG_LODWORD(
      cnf_data_ptr->client_id_and_evt_dereg_cnf.response_header.client_id);

  rsp_ptr->rsp_payload.response.client_id_and_evt_dereg.client_id_high =
    UIMDIAG_HIDWORD(
      cnf_data_ptr->client_id_and_evt_dereg_cnf.response_header.client_id);

  return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_proc_client_id_and_evt_dereg_cnf */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_PROC_SESSION_OPEN_CNF

DESCRIPTION
  Function to populate the response pointer with the confirmation data

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_proc_session_open_cnf(
  uimdiag_generic_rsp_type      *rsp_ptr,
  const mmgsdi_cnf_type         *cnf_data_ptr
)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cnf_data_ptr, rsp_ptr);

  (void)memset((void*)&rsp_ptr->rsp_payload.response.session_open,
    0x00,sizeof(uimdiag_mmgsdi_rsp_session_open_pyld_type));

   rsp_ptr->rsp_payload.response.session_open.session_id_low =
     UIMDIAG_LODWORD(
       (cnf_data_ptr->session_open_cnf.session_id));

    rsp_ptr->rsp_payload.response.session_open.session_id_high =
      UIMDIAG_HIDWORD(
        (cnf_data_ptr->session_open_cnf.session_id));

     rsp_ptr->rsp_payload.response.session_open.channel_id =
       (uint32)cnf_data_ptr->session_open_cnf.channel_id;
     rsp_ptr->rsp_payload.response.session_open.app_type   =
       (uint32)cnf_data_ptr->session_open_cnf.app_info.app_type;

     UIMDIAG_UTIL_RETURN_IF_OUT_OF_RANGE(
       cnf_data_ptr->session_open_cnf.app_info.aid.data_len,
        0,
        MMGSDI_MAX_AID_LEN);

     rsp_ptr->rsp_payload.response.session_open.aid_length =
       (uint32)cnf_data_ptr->session_open_cnf.app_info.aid.data_len;

     if(cnf_data_ptr->session_open_cnf.app_info.aid.data_len <=
         MMGSDI_MAX_AID_LEN)
     {
       memscpy((void *)(rsp_ptr->rsp_payload.response.session_open.aid_ptr),
       sizeof(rsp_ptr->rsp_payload.response.session_open.aid_ptr),
       cnf_data_ptr->session_open_cnf.app_info.aid.data_ptr,
       int32touint32(cnf_data_ptr->session_open_cnf.app_info.aid.data_len));
     }
     else
     {
        UIMDIAG_MSG_ERROR_0("Session Open CNF: Invalid AID data ptr");
        return UIMDIAG_ERROR;
     }

     UIMDIAG_UTIL_RETURN_IF_OUT_OF_RANGE(
      cnf_data_ptr->session_open_cnf.app_info.label.data_len,
      0,MMGSDI_MAX_AID_LEN);

     rsp_ptr->rsp_payload.response.session_open.label_len   =
       (uint32)cnf_data_ptr->session_open_cnf.app_info.label.data_len;

     if(cnf_data_ptr->session_open_cnf.app_info.label.data_len <=
        MMGSDI_MAX_AID_LEN)
      {
        memscpy((void *)(rsp_ptr->rsp_payload.response.session_open.label_ptr),
          sizeof(rsp_ptr->rsp_payload.response.session_open.label_ptr),
          cnf_data_ptr->session_open_cnf.app_info.label.data_ptr,
          int32touint32(cnf_data_ptr->session_open_cnf.app_info.label.data_len));
     }
     else
     {
       UIMDIAG_MSG_ERROR_0("Session Open CNF: Invalid Label data ptr");
       return UIMDIAG_ERROR;
     }
     return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_proc_session_open_cnf*/


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_PROC_SESSION_CLOSE_CNF

DESCRIPTION
  Function to populate the response pointer with the conformation data

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_proc_session_close_cnf(
  uimdiag_generic_rsp_type      *rsp_ptr,
  const mmgsdi_cnf_type         *cnf_data_ptr
)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cnf_data_ptr, rsp_ptr);

  (void)memset((void*)&rsp_ptr->rsp_payload.response.session_close,
    0x00,sizeof(uimdiag_mmgsdi_rsp_session_close_pyld_type));

   rsp_ptr->rsp_payload.response.session_close.session_id_low   =
     UIMDIAG_LODWORD(
       (cnf_data_ptr->session_close_cnf.session_id));

    rsp_ptr->rsp_payload.response.session_close.session_id_high =
      UIMDIAG_HIDWORD(
        (cnf_data_ptr->session_close_cnf.session_id));

    rsp_ptr->rsp_payload.response.session_close.app_type        =
       (uint32)cnf_data_ptr->session_close_cnf.app_info.app_type;

    UIMDIAG_UTIL_RETURN_IF_OUT_OF_RANGE(
      cnf_data_ptr->session_close_cnf.app_info.aid.data_len,
      0,
      MMGSDI_MAX_AID_LEN);

    rsp_ptr->rsp_payload.response.session_close.aid_length      =
       (uint32)cnf_data_ptr->session_close_cnf.app_info.aid.data_len;

    if(cnf_data_ptr->session_close_cnf.app_info.aid.data_len <=
       MMGSDI_MAX_AID_LEN)
      {
       memscpy((void *)(rsp_ptr->rsp_payload.response.session_close.aid_ptr),
       sizeof(rsp_ptr->rsp_payload.response.session_close.aid_ptr),
       cnf_data_ptr->session_close_cnf.app_info.aid.data_ptr,
       int32touint32(cnf_data_ptr->session_close_cnf.app_info.aid.data_len));
      }
    else
     {
       UIMDIAG_MSG_ERROR_0("Uimdiag Session Close CNF: Invalid AID data ptr");
       return UIMDIAG_ERROR;
     }

    UIMDIAG_UTIL_RETURN_IF_OUT_OF_RANGE(
      cnf_data_ptr->session_close_cnf.app_info.label.data_len,
      0,
      MMGSDI_MAX_AID_LEN);

    rsp_ptr->rsp_payload.response.session_close.label_len       =
      (uint32)cnf_data_ptr->session_close_cnf.app_info.label.data_len;

    if(cnf_data_ptr->session_close_cnf.app_info.label.data_len  <=
       MMGSDI_MAX_AID_LEN)
    {
      memscpy((void *)(rsp_ptr->rsp_payload.response.session_close.label_ptr),
        sizeof(rsp_ptr->rsp_payload.response.session_close.label_ptr),
        cnf_data_ptr->session_close_cnf.app_info.label.data_ptr,
        int32touint32(cnf_data_ptr->session_close_cnf.app_info.label.data_len));
     }
     else
     {
       UIMDIAG_MSG_ERROR_0("UimDiag Session close CNF: Invalid Label data ptr");
       return UIMDIAG_ERROR;
     }
     return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_proc_session_close_cnf */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_PROC_ACT_OR_SWITCH_PROV_CNF

DESCRIPTION
  Function to populate the response pointer with the conformation data

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type.

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_proc_act_or_switch_prov_cnf(
  uimdiag_generic_rsp_type      *rsp_ptr,
  const mmgsdi_cnf_type         *cnf_data_ptr
)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cnf_data_ptr, rsp_ptr);

  (void)memset(
    (void*)&rsp_ptr->rsp_payload.response.session_act_or_sw_prov,
      0x00, sizeof(uimdiag_mmgsdi_sessionid_type));

  rsp_ptr->rsp_payload.response.session_act_or_sw_prov.session_id_low =
    UIMDIAG_LODWORD(
      (cnf_data_ptr->session_activate_or_switch_provisioning_cnf.
                                                response_header.session_id));

  rsp_ptr->rsp_payload.response.session_act_or_sw_prov.session_id_high =
    UIMDIAG_HIDWORD(
      (cnf_data_ptr->session_activate_or_switch_provisioning_cnf.
                                                response_header.session_id));
  return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_proc_act_or_switch_prov_cnf */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_PROC_GET_INFO_CNF

DESCRIPTION
  Function to populate the response pointer with the conformation data

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_proc_get_info_cnf(
  uimdiag_generic_rsp_type      *rsp_ptr,
  const mmgsdi_cnf_type         *cnf_data_ptr
)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cnf_data_ptr, rsp_ptr);

  (void)memset((void*)&rsp_ptr->rsp_payload.response.session_get_info,
    0x00,sizeof(uimdiag_mmgsdi_rsp_session_get_info_pyld_type));

  rsp_ptr->rsp_payload.response.session_get_info.session_type    =
                     (uint32) cnf_data_ptr->session_get_info_cnf.session_type;

  rsp_ptr->rsp_payload.response.session_get_info.slot_id         =
    (uint32)cnf_data_ptr->session_get_info_cnf.slot_id;

  rsp_ptr->rsp_payload.response.session_get_info.channel_id         =
    (uint32)cnf_data_ptr->session_get_info_cnf.channel_id;

  rsp_ptr->rsp_payload.response.session_get_info.app_type        =
    (uint32)cnf_data_ptr->session_get_info_cnf.app_data.app_type;

  UIMDIAG_UTIL_RETURN_IF_OUT_OF_RANGE(
    cnf_data_ptr->session_get_info_cnf.app_data.aid.data_len,
    0,
    MMGSDI_MAX_AID_LEN);

  rsp_ptr->rsp_payload.response.session_get_info.aid_length      =
    int32touint32(cnf_data_ptr->session_get_info_cnf.app_data.aid.data_len);

  if(cnf_data_ptr->session_get_info_cnf.app_data.aid.data_len <=
     MMGSDI_MAX_AID_LEN)
    {
      memscpy((void *)(rsp_ptr->rsp_payload.response.session_get_info.aid_ptr),
      sizeof(rsp_ptr->rsp_payload.response.session_get_info.aid_ptr),
      cnf_data_ptr->session_get_info_cnf.app_data.aid.data_ptr,
      int32touint32(cnf_data_ptr->session_get_info_cnf.app_data.aid.data_len));
     }
   else
    {
      UIMDIAG_MSG_ERROR_0("Uimdiag Session Get info CNF: Invalid AID data ptr");
      return UIMDIAG_ERROR;
     }

   UIMDIAG_UTIL_RETURN_IF_OUT_OF_RANGE(
    cnf_data_ptr->session_get_info_cnf.app_data.label.data_len,
    0,
    MMGSDI_MAX_AID_LEN);

   rsp_ptr->rsp_payload.response.session_get_info.label_len      =
     int32touint32(cnf_data_ptr->session_get_info_cnf.app_data.label.data_len);

   if(cnf_data_ptr->session_get_info_cnf.app_data.label.data_len <=
      MMGSDI_MAX_AID_LEN)
   {
     memscpy((void *)(rsp_ptr->rsp_payload.response.session_get_info.label_ptr),
     sizeof(rsp_ptr->rsp_payload.response.session_get_info.label_ptr),
     cnf_data_ptr->session_get_info_cnf.app_data.label.data_ptr,
     int32touint32(cnf_data_ptr->session_get_info_cnf.app_data.label.data_len));
   }
   else
   {
      UIMDIAG_MSG_ERROR_0("Uimdiag Session get info CNF: Invalid Label data ptr");
      return UIMDIAG_ERROR;
   }
   return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_proc_get_info_cnf*/


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_PROC_GET_ALL_PIN_STATUS_CNF

DESCRIPTION
  Function to populate the response pointer with the confirmation data

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_proc_get_all_pin_status_cnf (
  uimdiag_generic_rsp_type      *rsp_ptr,
  const mmgsdi_cnf_type         *cnf_data_ptr)

{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cnf_data_ptr, rsp_ptr);

  (void)memset(
    (void*)&rsp_ptr->rsp_payload.response.session_get_all_pin_status,
    0x00,sizeof(uimdiag_mmgsdi_rsp_session_get_all_pin_stat_pyld_type));

  rsp_ptr->rsp_payload.response.session_get_all_pin_status.pin_id           =
    (uint32)cnf_data_ptr->get_all_pin_status_cnf.pin1.pin_id;

  rsp_ptr->rsp_payload.response.session_get_all_pin_status.pin_status       =
    (uint32)cnf_data_ptr->get_all_pin_status_cnf.pin1.status;

  rsp_ptr->rsp_payload.response.session_get_all_pin_status.pin_replacement  =
    (uint32)cnf_data_ptr->get_all_pin_status_cnf.pin1.pin_replacement;

  rsp_ptr->rsp_payload.response.session_get_all_pin_status.pin_num_retries  =
    (uint32)cnf_data_ptr->get_all_pin_status_cnf.pin1.num_retries;

  rsp_ptr->rsp_payload.response.session_get_all_pin_status.puk_num_retries  =
    (uint32)cnf_data_ptr->get_all_pin_status_cnf.pin1.num_unblock_retries;

  rsp_ptr->rsp_payload.response.session_get_all_pin_status.pin2_pin_id      =
    (uint32)cnf_data_ptr->get_all_pin_status_cnf.pin2.pin_id;

  rsp_ptr->rsp_payload.response.session_get_all_pin_status.pin2_status      =
    (uint32)cnf_data_ptr->get_all_pin_status_cnf.pin2.status;

  rsp_ptr->rsp_payload.response.session_get_all_pin_status.pin2_replacement =
    (uint32)cnf_data_ptr->get_all_pin_status_cnf.pin2.pin_replacement;

  rsp_ptr->rsp_payload.response.session_get_all_pin_status.pin2_num_retries =
    (uint32)cnf_data_ptr->get_all_pin_status_cnf.pin2.num_retries;

  rsp_ptr->rsp_payload.response.session_get_all_pin_status.puk2_num_retries =
    (uint32)cnf_data_ptr->get_all_pin_status_cnf.pin2.num_unblock_retries;

  rsp_ptr->rsp_payload.response.session_get_all_pin_status.upin_id          =
    (uint32)cnf_data_ptr->get_all_pin_status_cnf.universal_pin.pin_id;

  rsp_ptr->rsp_payload.response.session_get_all_pin_status.upin_status      =
    (uint32)cnf_data_ptr->get_all_pin_status_cnf.universal_pin.status;

  rsp_ptr->rsp_payload.response.session_get_all_pin_status.upin_replacement =
    (uint32)cnf_data_ptr->get_all_pin_status_cnf.universal_pin.pin_replacement;

  rsp_ptr->rsp_payload.response.session_get_all_pin_status.upin_num_retries =
    (uint32)cnf_data_ptr->get_all_pin_status_cnf.universal_pin.num_retries;

  rsp_ptr->rsp_payload.response.session_get_all_pin_status.upuk_num_retries =
    (uint32)cnf_data_ptr->get_all_pin_status_cnf.universal_pin.num_unblock_retries;

  rsp_ptr->rsp_payload.response.session_get_all_pin_status.hiddenkey_id =
    (uint32)cnf_data_ptr->get_all_pin_status_cnf.hiddenkey.pin_id;

  rsp_ptr->rsp_payload.response.session_get_all_pin_status.hiddenkey_status =
    (uint32)cnf_data_ptr->get_all_pin_status_cnf.hiddenkey.status;

  rsp_ptr->rsp_payload.response.session_get_all_pin_status.dir_index =
    (uint8)cnf_data_ptr->get_all_pin_status_cnf.dir_index;

  return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_proc_get_all_pin_status_cnf */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_PROC_PIN_OPERATION_CNF

DESCRIPTION
  Function to populate the response pointer with the confirmation data
  for verify pin, disable pin and enable pin

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_proc_pin_operation_cnf (
  uimdiag_generic_rsp_type      *rsp_ptr,
  const mmgsdi_cnf_type         *cnf_data_ptr)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cnf_data_ptr, rsp_ptr);

  (void)memset((void*)&rsp_ptr->rsp_payload.response.session_pin_operation,
    0x00,sizeof(uimdiag_mmgsdi_rsp_session_pin_operation_pyld_type));

  rsp_ptr->rsp_payload.response.session_pin_operation.pin_id          =
    (uint32)cnf_data_ptr->pin_operation_cnf.pin_info.pin_id;

  rsp_ptr->rsp_payload.response.session_pin_operation.pin_status      =
    (uint32)cnf_data_ptr->pin_operation_cnf.pin_info.status;

  rsp_ptr->rsp_payload.response.session_pin_operation.pin_replacement =
    (uint32)cnf_data_ptr->pin_operation_cnf.pin_info.pin_replacement;

  rsp_ptr->rsp_payload.response.session_pin_operation.pin_num_retries =
    (uint32)cnf_data_ptr->pin_operation_cnf.pin_info.num_retries;

  rsp_ptr->rsp_payload.response.session_pin_operation.puk_num_retries =
    (uint32)cnf_data_ptr->pin_operation_cnf.pin_info.num_unblock_retries;

  rsp_ptr->rsp_payload.response.session_pin_operation.pin_op =
    (uint32)cnf_data_ptr->pin_operation_cnf.pin_op;

  rsp_ptr->rsp_payload.response.session_pin_operation.vld_pin_num_retries =
    (uint32)cnf_data_ptr->pin_operation_cnf.pin_info.valid_num_retries;

  rsp_ptr->rsp_payload.response.session_pin_operation.vld_puk_num_retries =
    (uint32)cnf_data_ptr->pin_operation_cnf.pin_info.valid_num_unblock_retries;
  return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_proc_pin_operation_cnf  */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_PROC_GET_FILE_ATTR_CONF

DESCRIPTION
  Function to populate the response pointer with the confirmation data
  for get file attributes

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_proc_get_file_attr_cnf(
  uimdiag_generic_rsp_type      *rsp_ptr,
  const mmgsdi_cnf_type         *cnf_data_ptr
)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cnf_data_ptr, rsp_ptr);

  (void)memset((void*)&rsp_ptr->rsp_payload.response.session_get_file_attr,
    0x00,sizeof(uimdiag_mmgsdi_rsp_session_get_file_attr_pyld_type));

  rsp_ptr->rsp_payload.response.session_get_file_attr.access_method_type =
       (uint32)cnf_data_ptr->get_file_attr_cnf.access.access_method;

  if (rsp_ptr->rsp_payload.response.session_get_file_attr.access_method_type ==
     (uint32) MMGSDI_EF_ENUM_ACCESS)
  {
    rsp_ptr->rsp_payload.response.session_get_file_attr.ef_enum  =
    (uint32)uimdiag_util_convert_from_mmgsdi_items_enum(
    cnf_data_ptr->get_file_attr_cnf.access.file.file_enum);
  }

  if(rsp_ptr->rsp_payload.response.session_get_file_attr.access_method_type ==
    (uint32) MMGSDI_DF_ENUM_ACCESS)
  {
    rsp_ptr->rsp_payload.response.session_get_file_attr.df_enum  =
      (uint32)cnf_data_ptr->get_file_attr_cnf.access.file.df_enum;
  }

  if(rsp_ptr->rsp_payload.response.session_get_file_attr.access_method_type ==
    (uint32) MMGSDI_BY_PATH_ACCESS)
  {
    rsp_ptr->rsp_payload.response.session_get_file_attr.path_len  =
      (uint32)cnf_data_ptr->get_file_attr_cnf.access.file.path_type.path_len;

    if(rsp_ptr->rsp_payload.response.session_get_file_attr.path_len > 0 &&
       rsp_ptr->rsp_payload.response.session_get_file_attr.path_len < MMGSDI_MAX_PATH_LEN)
    {
      memscpy((void *)rsp_ptr->rsp_payload.response.session_get_file_attr.path_ptr,
        sizeof(rsp_ptr->rsp_payload.response.session_get_file_attr.path_ptr),
        cnf_data_ptr->get_file_attr_cnf.access.file.path_type.path_buf,
        cnf_data_ptr->get_file_attr_cnf.access.file.path_type.path_len * sizeof(uint16));
    }
  }

  if(rsp_ptr->rsp_payload.response.session_get_file_attr.access_method_type ==
                                              (uint32) MMGSDI_BY_APP_ID_ACCESS)
  {
    rsp_ptr->rsp_payload.response.session_get_file_attr.aid_len =
      (uint32)cnf_data_ptr->get_file_attr_cnf.access.file.app_id.data_len;

    if(rsp_ptr->rsp_payload.response.session_get_file_attr.aid_len > 0 &&
       rsp_ptr->rsp_payload.response.session_get_file_attr.aid_len < MMGSDI_MAX_AID_LEN)
    {
      memscpy((void *)rsp_ptr->rsp_payload.response.session_get_file_attr.aid_ptr,
        sizeof(rsp_ptr->rsp_payload.response.session_get_file_attr.aid_ptr),
        cnf_data_ptr->get_file_attr_cnf.access.file.app_id.data_ptr,
        MMGSDI_MAX_AID_LEN);
    }
  }

  rsp_ptr->rsp_payload.response.session_get_file_attr.file_size =
  (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_size;

  memscpy((void *)(rsp_ptr->rsp_payload.response.session_get_file_attr.file_id),
  sizeof(rsp_ptr->rsp_payload.response.session_get_file_attr.file_id),
  cnf_data_ptr->get_file_attr_cnf.file_attrib.file_id,
  sizeof(cnf_data_ptr->get_file_attr_cnf.file_attrib.file_id));

  rsp_ptr->rsp_payload.response.session_get_file_attr.file_type =
  (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_type;

  switch(rsp_ptr->rsp_payload.response.session_get_file_attr.file_type)
    {
    case MMGSDI_LINEAR_FIXED_FILE:
         rsp_ptr->rsp_payload.response.session_get_file_attr.cf_increase_allowed = 0;

         rsp_ptr->rsp_payload.response.session_get_file_attr.num_of_rec =
         (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.num_of_rec;

         rsp_ptr->rsp_payload.response.session_get_file_attr.rec_len =
         (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.rec_len;

         rsp_ptr->rsp_payload.response.session_get_file_attr.file_invalid_flg =
         (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.file_status.file_invalidated;

         rsp_ptr->rsp_payload.response.session_get_file_attr.rw_allowed_when_inv =
         (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.file_status.read_write_when_invalidated;

         rsp_ptr->rsp_payload.response.session_get_file_attr.inc_cond =
         (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.file_security.increase.protection_method;

         rsp_ptr->rsp_payload.response.session_get_file_attr.inc_no_pin =
         (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.file_security.increase.num_protection_pin;

         rsp_ptr->rsp_payload.response.session_get_file_attr.read_cond =
         (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.file_security.read.protection_method;

         rsp_ptr->rsp_payload.response.session_get_file_attr.read_no_pin =
         (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.file_security.read.num_protection_pin;

         rsp_ptr->rsp_payload.response.session_get_file_attr.write_cond =
         (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.file_security.write.protection_method;

         rsp_ptr->rsp_payload.response.session_get_file_attr.write_no_pin =
         (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.file_security.write.num_protection_pin;

         rsp_ptr->rsp_payload.response.session_get_file_attr.inact_cond =
         (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.file_security.invalidate_deactivate.protection_method;

         rsp_ptr->rsp_payload.response.session_get_file_attr.inact_no_pin =
         (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.file_security.invalidate_deactivate.num_protection_pin;

         rsp_ptr->rsp_payload.response.session_get_file_attr.act_cond =
         (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.file_security.rehabilitate_activate.protection_method;

         rsp_ptr->rsp_payload.response.session_get_file_attr.act_no_pin =
         (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.file_security.rehabilitate_activate.num_protection_pin;

         break;

    case  MMGSDI_TRANSPARENT_FILE:

          rsp_ptr->rsp_payload.response.session_get_file_attr.cf_increase_allowed = 0;

          rsp_ptr->rsp_payload.response.session_get_file_attr.num_of_rec = 0;

          rsp_ptr->rsp_payload.response.session_get_file_attr.rec_len = 0;

          rsp_ptr->rsp_payload.response.session_get_file_attr.file_invalid_flg =
      (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.transparent_file.file_status.file_invalidated;

      rsp_ptr->rsp_payload.response.session_get_file_attr.rw_allowed_when_inv =
      (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.transparent_file.file_status.read_write_when_invalidated;

      rsp_ptr->rsp_payload.response.session_get_file_attr.inc_cond =
          (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.transparent_file.file_security.increase.protection_method;

      rsp_ptr->rsp_payload.response.session_get_file_attr.inc_no_pin =
          (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.transparent_file.file_security.increase.num_protection_pin;

          rsp_ptr->rsp_payload.response.session_get_file_attr.read_cond =
          (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.transparent_file.file_security.read.protection_method;

          rsp_ptr->rsp_payload.response.session_get_file_attr.read_no_pin =
          (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.transparent_file.file_security.read.num_protection_pin;

          rsp_ptr->rsp_payload.response.session_get_file_attr.write_cond =
          (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.transparent_file.file_security.write.protection_method;

          rsp_ptr->rsp_payload.response.session_get_file_attr.write_no_pin =
          (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.transparent_file.file_security.write.num_protection_pin;

          rsp_ptr->rsp_payload.response.session_get_file_attr.inact_cond =
          (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.transparent_file.file_security.invalidate_deactivate.protection_method;

          rsp_ptr->rsp_payload.response.session_get_file_attr.inact_no_pin =
          (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.transparent_file.file_security.invalidate_deactivate.num_protection_pin;

          rsp_ptr->rsp_payload.response.session_get_file_attr.act_cond =
          (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.transparent_file.file_security.rehabilitate_activate.protection_method;

          rsp_ptr->rsp_payload.response.session_get_file_attr.act_no_pin =
          (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.transparent_file.file_security.rehabilitate_activate.num_protection_pin;

          break;

     case MMGSDI_CYCLIC_FILE:

          rsp_ptr->rsp_payload.response.session_get_file_attr.num_of_rec =
          (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.cyclic_file.num_of_rec;

          rsp_ptr->rsp_payload.response.session_get_file_attr.rec_len =
          (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.cyclic_file.rec_len;

          rsp_ptr->rsp_payload.response.session_get_file_attr.file_invalid_flg =
          (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.cyclic_file.file_status.file_invalidated;

          rsp_ptr->rsp_payload.response.session_get_file_attr.rw_allowed_when_inv =
          (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.cyclic_file.file_status.read_write_when_invalidated;

          rsp_ptr->rsp_payload.response.session_get_file_attr.cf_increase_allowed =
          (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.cyclic_file.increase_allowed;

          rsp_ptr->rsp_payload.response.session_get_file_attr.inc_cond =
          (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.cyclic_file.file_security.increase.protection_method;

          rsp_ptr->rsp_payload.response.session_get_file_attr.inc_no_pin =
          (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.cyclic_file.file_security.increase.num_protection_pin;

          rsp_ptr->rsp_payload.response.session_get_file_attr.read_cond =
          (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.cyclic_file.file_security.read.protection_method;

          rsp_ptr->rsp_payload.response.session_get_file_attr.read_no_pin =
          (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.cyclic_file.file_security.read.num_protection_pin;

          rsp_ptr->rsp_payload.response.session_get_file_attr.write_cond =
          (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.cyclic_file.file_security.write.protection_method;

          rsp_ptr->rsp_payload.response.session_get_file_attr.write_no_pin =
          (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.cyclic_file.file_security.write.num_protection_pin;

          rsp_ptr->rsp_payload.response.session_get_file_attr.inact_cond =
          (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.cyclic_file.file_security.invalidate_deactivate.protection_method;

          rsp_ptr->rsp_payload.response.session_get_file_attr.inact_no_pin =
          (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.cyclic_file.file_security.invalidate_deactivate.num_protection_pin;

          rsp_ptr->rsp_payload.response.session_get_file_attr.act_cond =
          (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.cyclic_file.file_security.rehabilitate_activate.protection_method;

          rsp_ptr->rsp_payload.response.session_get_file_attr.act_no_pin =
          (uint32)cnf_data_ptr->get_file_attr_cnf.file_attrib.file_info.cyclic_file.file_security.rehabilitate_activate.num_protection_pin;

          break;

     case MMGSDI_MASTER_FILE:
     case MMGSDI_DEDICATED_FILE:
       break;

     default:
       UIMDIAG_MSG_ERROR_0("Invalid file type passed");
       return UIMDIAG_ERROR;

  }
   return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_proc_get_file_attr_cnf*/

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_PROC_READ_RECORD_CONF

DESCRIPTION
  Function to populate the response pointer with the confirmation data
  for read record and read transparent

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_proc_read_record_cnf(
  uimdiag_generic_rsp_type      *rsp_ptr,
  const mmgsdi_cnf_type         *cnf_data_ptr
)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cnf_data_ptr, rsp_ptr);

  (void)memset((void*)&rsp_ptr->rsp_payload.response.session_read_record,
   0x00,sizeof(uimdiag_mmgsdi_rsp_session_read_record_pyld_type));

  rsp_ptr->rsp_payload.response.session_read_record.access_method_type =
    (uint32) cnf_data_ptr->read_cnf.access.access_method;

  if(rsp_ptr->rsp_payload.response.session_read_record.access_method_type ==
     (uint32) MMGSDI_EF_ENUM_ACCESS)
  {
    rsp_ptr->rsp_payload.response.session_read_record.ef_enum  =
     (uint32)uimdiag_util_convert_from_mmgsdi_items_enum(
       cnf_data_ptr->read_cnf.access.file.file_enum);
    rsp_ptr->rsp_payload.response.session_read_record.df_enum  = 0;
    rsp_ptr->rsp_payload.response.session_read_record.path_len = 0;
    rsp_ptr->rsp_payload.response.session_read_record.aid_len  = 0;
  }

  if(rsp_ptr->rsp_payload.response.session_read_record.access_method_type ==
     (uint32) MMGSDI_DF_ENUM_ACCESS)
  {
    rsp_ptr->rsp_payload.response.session_read_record.df_enum  =
    (uint32)cnf_data_ptr->read_cnf.access.file.df_enum;
  }

  if(rsp_ptr->rsp_payload.response.session_read_record.access_method_type ==
     (uint32) MMGSDI_BY_PATH_ACCESS)
  {
    rsp_ptr->rsp_payload.response.session_read_record.path_len  =
      (uint32)cnf_data_ptr->read_cnf.access.file.path_type.path_len;
    if(rsp_ptr->rsp_payload.response.session_read_record.path_len != 0)
    {
      memscpy((void *)(rsp_ptr->rsp_payload.response.session_read_record.path_ptr),
        sizeof(rsp_ptr->rsp_payload.response.session_read_record.path_ptr),
        cnf_data_ptr->read_cnf.access.file.path_type.path_buf,
        (sizeof(uint16)*rsp_ptr->rsp_payload.response.session_read_record.path_len));
    }
  }

  if(rsp_ptr->rsp_payload.response.session_read_record.access_method_type ==
     (uint32) MMGSDI_BY_APP_ID_ACCESS)
  {
    rsp_ptr->rsp_payload.response.session_read_record.aid_len =
      (uint32)cnf_data_ptr->read_cnf.access.file.app_id.data_len;
    if(rsp_ptr->rsp_payload.response.session_read_record.aid_len != 0)
    {
      memscpy((void *)(rsp_ptr->rsp_payload.response.session_read_record.aid_ptr),
        sizeof(rsp_ptr->rsp_payload.response.session_read_record.aid_ptr),
        cnf_data_ptr->read_cnf.access.file.app_id.data_ptr,
        rsp_ptr->rsp_payload.response.session_read_record.aid_len);
    }
  }

  rsp_ptr->rsp_payload.response.session_read_record.rec_num =
    (uint32)cnf_data_ptr->read_cnf.accessed_rec_num;

  rsp_ptr->rsp_payload.response.session_read_record.offset =
    (uint32)cnf_data_ptr->read_cnf.accessed_offset;

  rsp_ptr->rsp_payload.response.session_read_record.data_len =
    (uint32)cnf_data_ptr->read_cnf.read_data.data_len;

  if(rsp_ptr->rsp_payload.response.session_read_record.data_len != 0)
  {
    memscpy((void *)(rsp_ptr->rsp_payload.response.session_read_record.data_ptr),
      sizeof(rsp_ptr->rsp_payload.response.session_read_record.data_ptr),
      cnf_data_ptr->read_cnf.read_data.data_ptr,
      rsp_ptr->rsp_payload.response.session_read_record.data_len);
  }

  return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_proc_read_record_cnf*/


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_PROC_WRITE_RECORD_CONF

DESCRIPTION
  Function to populate the response pointer with the confirmation data
  for write record and write transparent

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_proc_write_record_cnf(
  uimdiag_generic_rsp_type      *rsp_ptr,
  const mmgsdi_cnf_type         *cnf_data_ptr
)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cnf_data_ptr, rsp_ptr);

  (void)memset((void*)&rsp_ptr->rsp_payload.response.session_write_record,
   0x00,sizeof(uimdiag_mmgsdi_rsp_session_write_record_pyld_type));

  rsp_ptr->rsp_payload.response.session_write_record.access_method_type =
  (uint32) cnf_data_ptr->write_cnf.access.access_method;

  if(rsp_ptr->rsp_payload.response.session_write_record.access_method_type ==
     (uint32) MMGSDI_EF_ENUM_ACCESS)
    {
     rsp_ptr->rsp_payload.response.session_write_record.ef_enum  =
     (uint32)uimdiag_util_convert_from_mmgsdi_items_enum(
       cnf_data_ptr->write_cnf.access.file.file_enum);
     rsp_ptr->rsp_payload.response.session_write_record.df_enum  = 0;
     rsp_ptr->rsp_payload.response.session_write_record.path_len = 0;
     rsp_ptr->rsp_payload.response.session_write_record.aid_len  = 0;
    }

  if(rsp_ptr->rsp_payload.response.session_write_record.access_method_type ==
     (uint32) MMGSDI_DF_ENUM_ACCESS)
    {
     rsp_ptr->rsp_payload.response.session_write_record.df_enum  =
     (uint32)cnf_data_ptr->write_cnf.access.file.df_enum;
    }

  if(rsp_ptr->rsp_payload.response.session_write_record.access_method_type ==
     (uint32) MMGSDI_BY_PATH_ACCESS)
    {
     rsp_ptr->rsp_payload.response.session_write_record.path_len  =
     (uint32)cnf_data_ptr->write_cnf.access.file.path_type.path_len;

     if(rsp_ptr->rsp_payload.response.session_write_record.path_len != 0)
      {
       memscpy((void *)(rsp_ptr->rsp_payload.response.session_write_record.path_ptr),
       sizeof(rsp_ptr->rsp_payload.response.session_write_record.path_ptr),
       cnf_data_ptr->write_cnf.access.file.path_type.path_buf,
       rsp_ptr->rsp_payload.response.session_write_record.path_len * sizeof(uint16));
      }
    }

  if(rsp_ptr->rsp_payload.response.session_write_record.access_method_type ==
     (uint32) MMGSDI_BY_APP_ID_ACCESS)
    {
     rsp_ptr->rsp_payload.response.session_write_record.aid_len =
     (uint32)cnf_data_ptr->write_cnf.access.file.app_id.data_len;

     if(rsp_ptr->rsp_payload.response.session_write_record.aid_len != 0)
      {
       memscpy((void *)(rsp_ptr->rsp_payload.response.session_write_record.aid_ptr),
       sizeof(rsp_ptr->rsp_payload.response.session_write_record.aid_ptr),
       cnf_data_ptr->write_cnf.access.file.app_id.data_ptr,
       rsp_ptr->rsp_payload.response.session_write_record.aid_len);
      }
    }

  rsp_ptr->rsp_payload.response.session_write_record.rec_num =
  (uint32)cnf_data_ptr->write_cnf.accessed_rec_num;

  rsp_ptr->rsp_payload.response.session_write_record.offset =
  (uint32)cnf_data_ptr->write_cnf.accessed_offset;

  rsp_ptr->rsp_payload.response.session_write_record.data_len =
  (uint32)cnf_data_ptr->write_cnf.written_data_len;

     return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_proc_write_record_cnf*/


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_REFRESH_CNF

DESCRIPTION
  Function to populate the response pointer with the confirmation data

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type.

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_refresh_cnf(
  uimdiag_generic_rsp_type      *rsp_ptr,
  const mmgsdi_cnf_type         *cnf_data_ptr
)
{
  UIMDIAG_UTIL_RETURN_IF_NULL(cnf_data_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(rsp_ptr);

  (void)memset(
    (void*)&rsp_ptr->rsp_payload.response.session_refresh,
      0x00, sizeof(uimdiag_mmgsdi_sessionid_type));

  rsp_ptr->rsp_payload.response.session_refresh.session_id_low =
    UIMDIAG_LODWORD((cnf_data_ptr->refresh_cnf.response_header.session_id));

  rsp_ptr->rsp_payload.response.session_refresh.session_id_high =
    UIMDIAG_HIDWORD(
      (cnf_data_ptr->refresh_cnf.response_header.session_id));
  return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_refresh_cnf */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_PERSO_CNF

DESCRIPTION
  Function to populate the response pointer with the confirmation data

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type.

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_perso_cnf(
  uimdiag_generic_rsp_type      *rsp_ptr,
  const mmgsdi_cnf_type         *cnf_data_ptr
)
{
  uimdiag_mmgsdi_rsp_session_perso_get_key_pyld_type * rsp_data_ptr = NULL;
  const mmgsdi_perso_get_feature_key_type             * perso_data_ptr = NULL;
  uint32                                                imei_length    = 0;

  UIMDIAG_UTIL_RETURN_IF_NULL(cnf_data_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(rsp_ptr);

  switch(cnf_data_ptr->session_perso_cnf.perso_action)
  {
    case MMGSDI_PERSO_ACTIVATE:
    case MMGSDI_PERSO_DEACTIVATE:
    case MMGSDI_PERSO_UNBLOCK:

      (void)memset(
        (void*)&rsp_ptr->rsp_payload.response.session_perso,
         0x00, sizeof(uimdiag_mmgsdi_rsp_session_perso_pyld_type));

      rsp_ptr->rsp_payload.response.session_perso.perso_action    =
        (uint32)cnf_data_ptr->session_perso_cnf.perso_action;

      rsp_ptr->rsp_payload.response.session_perso.perso_feature =
        (uint32)cnf_data_ptr->session_perso_cnf.
        perso_cnf_data.feature_data_cnf.perso_feature;

      rsp_ptr->rsp_payload.response.session_perso.num_retries =
        cnf_data_ptr->session_perso_cnf.
        perso_cnf_data.feature_data_cnf.num_retries;

      break;

    case MMGSDI_PERSO_GET_FEATURE_IND:
      (void)memset(
        (void*)&rsp_ptr->rsp_payload.response.session_perso_get_ind,
          0x00, sizeof(uimdiag_mmgsdi_rsp_session_perso_get_ind_pyld_type));

      rsp_ptr->rsp_payload.response.session_perso_get_ind.nw_ind_status =
        (uint32)cnf_data_ptr->session_perso_cnf.perso_cnf_data.
        get_feature_ind_cnf.nw_ind_status;

      rsp_ptr->rsp_payload.response.session_perso_get_ind.ns_ind_status =
        (uint32)cnf_data_ptr->session_perso_cnf.perso_cnf_data.
        get_feature_ind_cnf.ns_ind_status;

      rsp_ptr->rsp_payload.response.session_perso_get_ind.sp_ind_status =
        (uint32)cnf_data_ptr->session_perso_cnf.perso_cnf_data.
        get_feature_ind_cnf.sp_ind_status;

      rsp_ptr->rsp_payload.response.session_perso_get_ind.cp_ind_status =
        (uint32)cnf_data_ptr->session_perso_cnf.perso_cnf_data.
        get_feature_ind_cnf.cp_ind_status;

      rsp_ptr->rsp_payload.response.session_perso_get_ind.sim_ind_status =
        (uint32)cnf_data_ptr->session_perso_cnf.perso_cnf_data.
        get_feature_ind_cnf.sim_ind_status;

      rsp_ptr->rsp_payload.response.session_perso_get_ind.ruim_nw1_ind =
        (uint32)cnf_data_ptr->session_perso_cnf.perso_cnf_data.
        get_feature_ind_cnf.ruim_nw1_ind_status;

      rsp_ptr->rsp_payload.response.session_perso_get_ind.ruim_nw2_ind =
        (uint32)cnf_data_ptr->session_perso_cnf.perso_cnf_data.
        get_feature_ind_cnf.ruim_nw2_ind_status;

      rsp_ptr->rsp_payload.response.session_perso_get_ind.ruim_hrpd_ind =
        (uint32)cnf_data_ptr->session_perso_cnf.perso_cnf_data.
        get_feature_ind_cnf.ruim_hrpd_ind_status;

      rsp_ptr->rsp_payload.response.session_perso_get_ind.ruim_sp_ind =
        (uint32)cnf_data_ptr->session_perso_cnf.perso_cnf_data.
        get_feature_ind_cnf.ruim_sp_ind_status;

      rsp_ptr->rsp_payload.response.session_perso_get_ind.ruim_cp_ind =
        (uint32)cnf_data_ptr->session_perso_cnf.perso_cnf_data.
        get_feature_ind_cnf.ruim_cp_ind_status;

      rsp_ptr->rsp_payload.response.session_perso_get_ind.ruim_ruim_ind =
        (uint32)cnf_data_ptr->session_perso_cnf.perso_cnf_data.
        get_feature_ind_cnf.ruim_ruim_ind_status;

      break;

    case MMGSDI_PERSO_GET_UNBLOCK_DCK_RETRIES:
    case MMGSDI_PERSO_GET_DCK_RETRIES:
      (void)memset(
        (void*)&rsp_ptr->rsp_payload.response.session_perso_get_dck_retries,
          0x00, sizeof(uimdiag_mmgsdi_rsp_session_perso_get_dck_retries_pyld_type));

      rsp_ptr->rsp_payload.response.session_perso_get_dck_retries.unblock_retries =
        (boolean)cnf_data_ptr->session_perso_cnf.perso_cnf_data.
        dck_num_retries_cnf.unblock_retries;

      rsp_ptr->rsp_payload.response.session_perso_get_dck_retries.nw_num_retries =
        int32touint32(cnf_data_ptr->session_perso_cnf.perso_cnf_data.
        dck_num_retries_cnf.nw_num_retries);

      rsp_ptr->rsp_payload.response.session_perso_get_dck_retries.ns_num_retries =
        int32touint32(cnf_data_ptr->session_perso_cnf.perso_cnf_data.
        dck_num_retries_cnf.ns_num_retries);

      rsp_ptr->rsp_payload.response.session_perso_get_dck_retries.sp_num_retries =
        int32touint32(cnf_data_ptr->session_perso_cnf.perso_cnf_data.
        dck_num_retries_cnf.sp_num_retries);

      rsp_ptr->rsp_payload.response.session_perso_get_dck_retries.cp_num_retries =
        int32touint32(cnf_data_ptr->session_perso_cnf.perso_cnf_data.
        dck_num_retries_cnf.cp_num_retries);

      rsp_ptr->rsp_payload.response.session_perso_get_dck_retries.sim_num_retries =
        int32touint32(cnf_data_ptr->session_perso_cnf.perso_cnf_data.
        dck_num_retries_cnf.sim_num_retries);

      rsp_ptr->rsp_payload.response.session_perso_get_dck_retries.ruim_nw1_num_retries =
        int32touint32(cnf_data_ptr->session_perso_cnf.perso_cnf_data.
        dck_num_retries_cnf.ruim_nw1_num_retries);

      rsp_ptr->rsp_payload.response.session_perso_get_dck_retries.ruim_nw2_num_retries =
        int32touint32(cnf_data_ptr->session_perso_cnf.perso_cnf_data.
        dck_num_retries_cnf.ruim_nw2_num_retries);

      rsp_ptr->rsp_payload.response.session_perso_get_dck_retries.ruim_hrpd_num_retries =
        int32touint32(cnf_data_ptr->session_perso_cnf.perso_cnf_data.
        dck_num_retries_cnf.ruim_hrpd_num_retries);

      rsp_ptr->rsp_payload.response.session_perso_get_dck_retries.ruim_sp_num_retries =
        int32touint32(cnf_data_ptr->session_perso_cnf.perso_cnf_data.
        dck_num_retries_cnf.ruim_sp_num_retries);

      rsp_ptr->rsp_payload.response.session_perso_get_dck_retries.ruim_cp_num_retries =
        int32touint32(cnf_data_ptr->session_perso_cnf.perso_cnf_data.
        dck_num_retries_cnf.ruim_cp_num_retries);

      rsp_ptr->rsp_payload.response.session_perso_get_dck_retries.ruim_ruim_num_retries =
        int32touint32(cnf_data_ptr->session_perso_cnf.perso_cnf_data.
        dck_num_retries_cnf.ruim_ruim_num_retries);
      break;

    case MMGSDI_PERSO_LOCK_DOWN:
      (void)memset(
        (void*)&rsp_ptr->rsp_payload.response.session_perso_lock_dwn_enable_ltd_access,
          0x00, sizeof(uimdiag_mmgsdi_rsp_session_perso_lock_dwn_enable_ltd_access_pyld_type));

      rsp_ptr->rsp_payload.response.session_perso_lock_dwn_enable_ltd_access.action_locked =
        (uint32)cnf_data_ptr->session_perso_cnf.perso_cnf_data.lock_down_cnf.locked_command;
    break;

    case MMGSDI_PERSO_SET_FEATURE_DATA:
    (void)memset(
      (void*)&rsp_ptr->rsp_payload.response.session_perso,
       0x00, sizeof(uimdiag_mmgsdi_sessionid_type));

     rsp_ptr->rsp_payload.response.session_perso_data.session_id_low =
      UIMDIAG_LODWORD(
        (cnf_data_ptr->session_perso_cnf.response_header.session_id));

    rsp_ptr->rsp_payload.response.session_perso_data.session_id_high =
      UIMDIAG_HIDWORD(
        (cnf_data_ptr->session_perso_cnf.response_header.session_id));

    break;

  case MMGSDI_PERSO_GET_FEATURE_KEY:
    (void)memset(
      (void*)&rsp_ptr->rsp_payload.response.session_perso_get_key,
        0x00, sizeof(uimdiag_mmgsdi_rsp_session_perso_get_key_pyld_type));

    rsp_data_ptr = &rsp_ptr->rsp_payload.response.session_perso_get_key;
    perso_data_ptr = &cnf_data_ptr->session_perso_cnf.perso_cnf_data.get_key_cnf;

    rsp_ptr->rsp_payload.response.session_perso_get_key.feature      =
      (uint32)cnf_data_ptr->session_perso_cnf.perso_cnf_data.get_key_cnf.feature;

    /* Returned data has IMEI and Key in format IMEI:KEY */
    while((*(perso_data_ptr->key.data_ptr + rsp_data_ptr->imei_length)
                                     != UIMDIAG_IMEI_KEY_SEPARATOR) &&
          (rsp_data_ptr->imei_length <= (uint32)perso_data_ptr->key.data_len))
    {
      rsp_data_ptr->imei_length += 1;
    }

    if(rsp_data_ptr->imei_length >= (uint32)perso_data_ptr->key.data_len)
    {
      /* Command response data is not in the expected format */
      UIMDIAG_MSG_ERROR_0("Invalid Data Returned by PERSO GET FEATURE KEY Command");
      rsp_ptr->rsp_payload.mmgsdi_status = MMGSDI_ERROR;
      return UIMDIAG_ERROR;
    }

    /* Set Key Length taking into account the ':' */
    rsp_data_ptr->key_length = ((perso_data_ptr->key.data_len -
                                                 rsp_data_ptr->imei_length) - 1);

    imei_length = int32touint32(rsp_data_ptr->imei_length);

    if(rsp_data_ptr->imei_length > UIMDIAG_MAX_IMEI_LENGTH)
    {
      UIMDIAG_MSG_ERROR_1("IMEI length greater than UIMDIAG_MAX_IMEI_LENGTH: 0x%x",
                          rsp_data_ptr->imei_length);
      imei_length = UIMDIAG_MAX_IMEI_LENGTH;
    }

    memscpy((void *)rsp_data_ptr->imei_data_ptr,
           sizeof(rsp_data_ptr->imei_data_ptr),
           (const char *)perso_data_ptr->key.data_ptr,
           int32touint32(imei_length));

    if(rsp_data_ptr->key_length > UIMDIAG_MAX_PERSO_KEY_LENGTH)
    {
      UIMDIAG_MSG_ERROR_1("Feature key length greater than UIMDIAG_MAX_PERSO_KEY_LENGTH: 0x%x",
                          rsp_data_ptr->key_length);

      rsp_data_ptr->key_length = UIMDIAG_MAX_PERSO_KEY_LENGTH;
    }

    if(rsp_data_ptr->key_length > 0)
    {
      memscpy((void *)rsp_data_ptr->key_data_ptr,
             sizeof(rsp_data_ptr->key_data_ptr),
             (const char *)(perso_data_ptr->key.data_ptr + rsp_data_ptr->imei_length + 1),
             int32touint32(rsp_data_ptr->key_length));
    }

    rsp_data_ptr->imei_length = imei_length;
    break;

  case MMGSDI_PERSO_GET_FEATURE_DATA:
    (void)memset(
      (void*)&rsp_ptr->rsp_payload.response.session_perso_get_data,
       0x00, sizeof(uimdiag_mmgsdi_rsp_session_perso_get_data_pyld_type));

    rsp_ptr->rsp_payload.response.session_perso_get_data.perso_action =
      (uint32)cnf_data_ptr->session_perso_cnf.perso_action;
    rsp_ptr->rsp_payload.response.session_perso_get_data.data_len =
      (uint32)cnf_data_ptr->session_perso_cnf.perso_cnf_data.get_data_cnf.data_len;

    if(rsp_ptr->rsp_payload.response.session_perso_get_data.data_len != 0)
    {
      memscpy((void *)(rsp_ptr->rsp_payload.response.session_perso_get_data.data_ptr),
        rsp_ptr->rsp_payload.response.session_perso_get_data.data_len,
        (const char *)cnf_data_ptr->session_perso_cnf.perso_cnf_data.get_data_cnf.data_ptr,
        int32touint32(rsp_ptr->rsp_payload.response.session_perso_get_data.data_len));
    }
    break;

  default:
      UIMDIAG_MSG_ERROR_0("Invalid Perso Action");
      return UIMDIAG_ERROR;
    }

  return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_perso_cnf */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_DEACTIVATE_PROV_CNF

DESCRIPTION
  Function to populate the response pointer with the confirmation data

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type.

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_session_deactivate_prov_cnf(
  uimdiag_generic_rsp_type      *rsp_ptr,
  const mmgsdi_cnf_type         *cnf_data_ptr
)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cnf_data_ptr, rsp_ptr);

  (void)memset(
    (void*)&rsp_ptr->rsp_payload.response.session_deact_prov,
      0x00, sizeof(uimdiag_mmgsdi_rsp_session_deact_prov_pyld_type));

  rsp_ptr->rsp_payload.response.session_deact_prov.session_id_low =
    UIMDIAG_LODWORD((cnf_data_ptr->session_deactivate_cnf.response_header.session_id));

  rsp_ptr->rsp_payload.response.session_deact_prov.session_id_high =
    UIMDIAG_HIDWORD(
      (cnf_data_ptr->session_deactivate_cnf.response_header.session_id));

  rsp_ptr->rsp_payload.response.session_deact_prov.app_type   =
    (uint32)cnf_data_ptr->session_deactivate_cnf.app_id.app_type;

  UIMDIAG_UTIL_RETURN_IF_OUT_OF_RANGE(
    cnf_data_ptr->session_deactivate_cnf.app_id.aid.data_len,
      0, MMGSDI_MAX_AID_LEN-1);

  rsp_ptr->rsp_payload.response.session_deact_prov.aid_length =
    (uint32)cnf_data_ptr->session_deactivate_cnf.app_id.aid.data_len;

  if(cnf_data_ptr->session_deactivate_cnf.app_id.aid.data_len <=
       MMGSDI_MAX_AID_LEN)
  {
    memscpy((void *)(rsp_ptr->rsp_payload.response.session_deact_prov.aid_ptr),
      sizeof(rsp_ptr->rsp_payload.response.session_deact_prov.aid_ptr),
      cnf_data_ptr->session_deactivate_cnf.app_id.aid.data_ptr,
      int32touint32(cnf_data_ptr->session_deactivate_cnf.app_id.aid.data_len));
  }

  UIMDIAG_UTIL_RETURN_IF_OUT_OF_RANGE(
    cnf_data_ptr->session_deactivate_cnf.app_id.label.data_len,
    0,MMGSDI_MAX_AID_LEN);

  rsp_ptr->rsp_payload.response.session_deact_prov.label_len   =
    (uint32)cnf_data_ptr->session_deactivate_cnf.app_id.label.data_len;

  if(cnf_data_ptr->session_deactivate_cnf.app_id.label.data_len <=
       MMGSDI_MAX_AID_LEN)
  {
    memscpy((void *)(rsp_ptr->rsp_payload.response.session_deact_prov.label_ptr),
      sizeof(rsp_ptr->rsp_payload.response.session_deact_prov.label_ptr),
      cnf_data_ptr->session_deactivate_cnf.app_id.label.data_ptr,
      int32touint32(cnf_data_ptr->session_deactivate_cnf.app_id.label.data_len));
  }

  return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_session_deactivate_prov_cnf */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_PROC_SESSION_JCDMA_GET_CARD_INFO_CNF

DESCRIPTION
  Function to populate the response pointer with the confirmation data

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type.

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_proc_session_jcdma_get_card_info_cnf(
  uimdiag_generic_rsp_type      *rsp_ptr,
  const mmgsdi_cnf_type         *cnf_data_ptr
)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cnf_data_ptr, rsp_ptr);

  (void)memset(
    (void*)&rsp_ptr->rsp_payload.response.session_jcdma_get_card_info,
      0x00, sizeof(uimdiag_mmgsdi_rsp_session_jcdma_get_card_info_pyld_type));

  rsp_ptr->rsp_payload.response.session_jcdma_get_card_info.card_info =
        (uint32)cnf_data_ptr->get_jcdma_card_info_cnf.card_info;

  rsp_ptr->rsp_payload.response.session_jcdma_get_card_info.me_esn_change =
        (uint32)cnf_data_ptr->get_jcdma_card_info_cnf.me_esn_change;

  return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_proc_session_jcdma_get_card_info_cnf */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_GET_APP_CAP_CNF

DESCRIPTION
  Function to populate the response pointer with the confirmation data

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type.

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_session_get_app_cap_cnf(
  uimdiag_generic_rsp_type      *rsp_ptr,
  const mmgsdi_session_app_capabilities_cnf_type         *session_app_cap_cnf_ptr
)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(session_app_cap_cnf_ptr, rsp_ptr);

  (void)memset(
    (void*)&rsp_ptr->rsp_payload.response.session_get_app_cap,
      0x00, sizeof(uimdiag_mmgsdi_rsp_session_get_app_cap_pyld_type));

  rsp_ptr->rsp_payload.response.session_get_app_cap.fdn_enabled       =
    session_app_cap_cnf_ptr->fdn_enabled;

  rsp_ptr->rsp_payload.response.session_get_app_cap.bdn_enabled       =
    session_app_cap_cnf_ptr->bdn_enabled;

  rsp_ptr->rsp_payload.response.session_get_app_cap.acl_enabled       =
    session_app_cap_cnf_ptr->acl_enabled;

  rsp_ptr->rsp_payload.response.session_get_app_cap.imsi_invalidated  =
    session_app_cap_cnf_ptr->imsi_invalidated;


  return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_session_get_app_cap_cnf */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_GET_CPHS_INFO_CNF

DESCRIPTION
  Function to populate the response pointer with the confirmation data

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type.

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_session_get_cphs_info_cnf(
  uimdiag_generic_rsp_type      *rsp_ptr,
  const mmgsdi_cnf_type         *cnf_data_ptr
)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cnf_data_ptr, rsp_ptr);

  (void)memset(
    (void*)&rsp_ptr->rsp_payload.response.session_get_cphs_info,
      0x00, sizeof(uimdiag_mmgsdi_rsp_session_get_cphs_info_pyld_type));

  rsp_ptr->rsp_payload.response.session_get_cphs_info.session_id_low    =
    UIMDIAG_LODWORD(
      (cnf_data_ptr->session_get_cphs_info_cnf.response_header.session_id));

  rsp_ptr->rsp_payload.response.session_get_cphs_info.session_id_high   =
    UIMDIAG_HIDWORD(
      (cnf_data_ptr->session_get_cphs_info_cnf.response_header.session_id));

  rsp_ptr->rsp_payload.response.session_get_cphs_info.cphs_capable       =
    cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_capable;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.phase       =
    cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_service.phase;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.csp       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_service.csp;


  rsp_ptr->rsp_payload.response.session_get_cphs_info.sst       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_service.sst;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.mbn       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_service.mbn;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.ops       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_service.ops;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.ifn       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_service.ifn;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.call_offering_CFU       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.call_offering_CFU;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.call_offering_CFB       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.call_offering_CFB;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.call_offering_CFNRy       =
    (uint32)
      cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.call_offering_CFNRy;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.call_offering_CFNRc       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.call_offering_CFNRc;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.call_offering_CT       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.call_offering_CT;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.call_restrict_BOAC       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.call_restrict_BOAC;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.call_restrict_BOIC       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.call_restrict_BOIC;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.call_restrict_BOIC_exHC       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.call_restrict_BOIC_exHC;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.call_restrict_BAIC       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.call_restrict_BAIC;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.call_restrict_BIC_Roam       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.call_restrict_BIC_Roam;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.other_supps_MPTY       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.other_supps_MPTY;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.other_supps_CUG       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.other_supps_CUG;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.other_supps_AoC       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.other_supps_AoC;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.other_supps_Pref_CUG       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.other_supps_Pref_CUG;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.other_supps_CUG_OA       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.other_supps_CUG_OA;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.call_completion_HOLD       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.call_completion_HOLD;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.call_completion_CW       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.call_completion_CW;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.call_completion_CCBS       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.call_completion_CCBS;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.call_completion_User_Signalling       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.call_completion_User_Signalling;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.teleservices_SM_MT       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.teleservices_SM_MT;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.teleservices_SM_MO       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.teleservices_SM_MO;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.teleservices_SM_CB       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.teleservices_SM_CB;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.teleservices_Reply_Path       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.teleservices_Reply_Path;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.teleservices_Del_Conf       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.teleservices_Del_Conf;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.teleservices_Protocol_ID       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.teleservices_Protocol_ID;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.teleservices_Validity_Period       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.teleservices_Validity_Period;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.cphs_teleservices_als       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.cphs_teleservices_als;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.cphs_features_SST       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.cphs_features_SST;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.number_identify_CLIP       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.number_identify_CLIP;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.number_identify_CoLR       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.number_identify_CoLR;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.number_identify_CoLP       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.number_identify_CoLP;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.number_identify_MCI       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.number_identify_MCI;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.number_identify_CLI_send       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.number_identify_CLI_send;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.number_identify_CLI_block       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.number_identify_CLI_block;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.p2plus_services_GPRS       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.p2plus_services_GPRS;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.p2plus_services_HSCSD       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.p2plus_services_HSCSD;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.p2plus_services_Voice_Group_call       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.p2plus_services_Voice_Group_call;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.p2plus_services_Voice_Broadcast_Svc       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.p2plus_services_Voice_Broadcast_Svc;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.p2plus_services_Muli_Sub_Profile       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.p2plus_services_Muli_Sub_Profile;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.p2plus_services_Multiband       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.p2plus_services_Multiband;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.value_added_PLMN_MODE       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.value_added_PLMN_MODE;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.value_added_VPS       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.value_added_VPS;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.value_added_SM_MO_PAGING       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.value_added_SM_MO_PAGING;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.value_added_SM_MO_EMAIL       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.value_added_SM_MO_EMAIL;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.value_added_FAX       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.value_added_FAX;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.value_added_DATA       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.value_added_DATA;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.valued_added_Language       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.valued_added_Language;

  rsp_ptr->rsp_payload.response.session_get_cphs_info.information_numbers       =
    (uint32)cnf_data_ptr->session_get_cphs_info_cnf.cphs_info.cphs_csp.information_numbers;

  return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_session_get_cphs_info_cnf */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_ISIM_AUTH_CNF

DESCRIPTION
  Function to populate the response pointer with the confirmation data

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_session_isim_auth_cnf (
  uimdiag_generic_rsp_type      *rsp_ptr,
  const mmgsdi_cnf_type         *cnf_data_ptr
)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cnf_data_ptr, rsp_ptr);

  (void)memset((void*)&rsp_ptr->rsp_payload.response.session_isim_auth,
    0x00,sizeof(uimdiag_mmgsdi_rsp_session_isim_auth_pyld_type));

  UIMDIAG_UTIL_RETURN_IF_OUT_OF_RANGE(
    cnf_data_ptr->isim_auth_cnf.res.data_len,
      0, UIMDIAG_ISIM_DATA_LEN);

  rsp_ptr->rsp_payload.response.session_isim_auth.res_length =
    (uint32)cnf_data_ptr->isim_auth_cnf.res.data_len;

  if(cnf_data_ptr->isim_auth_cnf.res.data_len <=
       UIMDIAG_ISIM_DATA_LEN-1)
  {
    memscpy((void *)(rsp_ptr->rsp_payload.response.session_isim_auth.res_ptr),
      sizeof(rsp_ptr->rsp_payload.response.session_isim_auth.res_ptr),
      cnf_data_ptr->isim_auth_cnf.res.data_ptr,
      int32touint32(cnf_data_ptr->isim_auth_cnf.res.data_len));
  }
  else
  {
    UIMDIAG_MSG_ERROR_0("ISIM Authenticate CNF: Invalid RES data ptr");
    return UIMDIAG_ERROR;
  }

  UIMDIAG_UTIL_RETURN_IF_OUT_OF_RANGE(
    cnf_data_ptr->isim_auth_cnf.ck.data_len,
      0,UIMDIAG_ISIM_DATA_LEN);

  rsp_ptr->rsp_payload.response.session_isim_auth.ck_len   =
    (uint32)cnf_data_ptr->isim_auth_cnf.ck.data_len;

  if(cnf_data_ptr->isim_auth_cnf.ck.data_len <=
       UIMDIAG_ISIM_DATA_LEN-1)
  {
    memscpy((void *)(rsp_ptr->rsp_payload.response.session_isim_auth.ck_ptr),
      sizeof(rsp_ptr->rsp_payload.response.session_isim_auth.ck_ptr),
      cnf_data_ptr->isim_auth_cnf.ck.data_ptr,
        int32touint32(cnf_data_ptr->isim_auth_cnf.ck.data_len));
  }
  else
  {
    UIMDIAG_MSG_ERROR_0("ISIM Authenticate CNF: Invalid CK data ptr");
    return UIMDIAG_ERROR;
  }

  UIMDIAG_UTIL_RETURN_IF_OUT_OF_RANGE(
    cnf_data_ptr->isim_auth_cnf.ik.data_len,
      0,UIMDIAG_ISIM_DATA_LEN);

  rsp_ptr->rsp_payload.response.session_isim_auth.ik_len   =
    (uint32)cnf_data_ptr->isim_auth_cnf.ik.data_len;

  if(cnf_data_ptr->isim_auth_cnf.ik.data_len <=
       UIMDIAG_ISIM_DATA_LEN-1)
  {
    memscpy((void *)(rsp_ptr->rsp_payload.response.session_isim_auth.ik_ptr),
      sizeof(rsp_ptr->rsp_payload.response.session_isim_auth.ik_ptr),
      cnf_data_ptr->isim_auth_cnf.ik.data_ptr,
        int32touint32(cnf_data_ptr->isim_auth_cnf.ik.data_len));
  }
  else
  {
    UIMDIAG_MSG_ERROR_0("ISIM Authenticate CNF: Invalid IK data ptr");
    return UIMDIAG_ERROR;
  }

  UIMDIAG_UTIL_RETURN_IF_OUT_OF_RANGE(
    cnf_data_ptr->isim_auth_cnf.auts.data_len,
      0,UIMDIAG_ISIM_DATA_LEN);

  rsp_ptr->rsp_payload.response.session_isim_auth.auts_len   =
    (uint32)cnf_data_ptr->isim_auth_cnf.auts.data_len;

  if(cnf_data_ptr->isim_auth_cnf.auts.data_len <=
       UIMDIAG_ISIM_DATA_LEN-1)
  {
    memscpy((void *)(rsp_ptr->rsp_payload.response.session_isim_auth.auts_ptr),
      sizeof(rsp_ptr->rsp_payload.response.session_isim_auth.auts_ptr),
      cnf_data_ptr->isim_auth_cnf.auts.data_ptr,
        int32touint32(cnf_data_ptr->isim_auth_cnf.auts.data_len));
  }
  else
  {
    UIMDIAG_MSG_ERROR_0("ISIM Authenticate CNF: Invalid auts data ptr");
    return UIMDIAG_ERROR;
  }
  return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_session_isim_auth_cnf*/


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_SEND_CARD_STATUS_CNF

DESCRIPTION
  Function to populate the response pointer with the confirmation data

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_session_send_card_status_cnf (
  uimdiag_generic_rsp_type      *rsp_ptr,
  const mmgsdi_cnf_type         *cnf_data_ptr,
  uint32                         rsp_len
)
{
  uint32                         card_status_data_len = 0;

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cnf_data_ptr, rsp_ptr);

  rsp_ptr->rsp_payload.response.session_send_crd_status.status_data_len =
    (uint32)cnf_data_ptr->status_cnf.status_data.data_len;

  if (rsp_len > (sizeof(uimdiag_cmd_rsp_hdr_type) +
                 sizeof(mmgsdi_return_enum_type) +
                 sizeof(rsp_ptr->rsp_payload.response.session_send_crd_status.status_data_len)))
  {
    /* Calculating the actual length of data */
    card_status_data_len = rsp_len -
      sizeof(uimdiag_cmd_rsp_hdr_type) -
      sizeof(mmgsdi_return_enum_type) -
      sizeof(rsp_ptr->rsp_payload.response.session_send_crd_status.status_data_len);
  }
  else
  {
    return UIMDIAG_ERROR;
  }

  if(cnf_data_ptr->status_cnf.status_data.data_len > 0)
  {
    memscpy((void *)
    (rsp_ptr->rsp_payload.response.session_send_crd_status.status_data),
     card_status_data_len,
     cnf_data_ptr->status_cnf.status_data.data_ptr,
     int32touint32(cnf_data_ptr->status_cnf.status_data.data_len));
  }

  return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_session_send_card_status_cnf */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_DISABLE_SERVICE_CNF

DESCRIPTION
  Function to populate the response pointer with the conformation data for
  disable service

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type.

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_session_disable_service_cnf(
  uimdiag_generic_rsp_type      *rsp_ptr,
  const mmgsdi_cnf_type         *cnf_data_ptr
)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cnf_data_ptr, rsp_ptr);

  (void)memset(
    (void*)&rsp_ptr->rsp_payload.response.session_disable_srv,
      0x00, sizeof(uimdiag_mmgsdi_rsp_session_srv_pyld_type));

  rsp_ptr->rsp_payload.response.session_disable_srv.service_type =
    (uint32)cnf_data_ptr->session_disable_service_cnf.service_type;

  return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_session_disable_service_cnf */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_ENABLE_SERVICE_CNF

DESCRIPTION
  Function to populate the response pointer with the conformation data for
  enable service

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type.

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_session_enable_service_cnf(
  uimdiag_generic_rsp_type      *rsp_ptr,
  const mmgsdi_cnf_type         *cnf_data_ptr
)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cnf_data_ptr, rsp_ptr);

  (void)memset(
    (void*)&rsp_ptr->rsp_payload.response.session_enable_srv,
      0x00, sizeof(uimdiag_mmgsdi_rsp_session_srv_pyld_type));

  rsp_ptr->rsp_payload.response.session_enable_srv.service_type =
    (uint32)cnf_data_ptr->session_enable_service_cnf.service_type;

  return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_session_enable_service_cnf */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SRV_AVAIL_CNF

DESCRIPTION
  Function to populate the response pointer with the conformation data

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type.

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_srv_avail_cnf(
  uimdiag_generic_rsp_type      *rsp_ptr,
  const mmgsdi_cnf_type         *cnf_data_ptr
)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cnf_data_ptr, rsp_ptr);

  (void)memset(
    (void*)&rsp_ptr->rsp_payload.response.session_is_srv_avail,
      0x00, sizeof(uimdiag_mmgsdi_rsp_session_is_srv_avail_pyld_type));

  rsp_ptr->rsp_payload.response.session_is_srv_avail.srv_available =
    (uint32)cnf_data_ptr->srv_available_cnf.srv_available;

  return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_srv_avail_cnf */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_SEARCH_CNF

DESCRIPTION
  Function to populate the response pointer with the confirmation data
  for mmgsdi_seesion_seek as well as mmgsdi_session_search

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_session_search_cnf (
  uimdiag_generic_rsp_type      *rsp_ptr,
  const mmgsdi_cnf_type         *cnf_data_ptr
)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cnf_data_ptr, rsp_ptr);

  (void)memset((void*)&rsp_ptr->rsp_payload.response.session_search_rsp,
    0x00,sizeof(uimdiag_mmgsdi_rsp_session_search_seek_pyld_type));

  rsp_ptr->rsp_payload.response.session_search_rsp.access_method =
    (uint32)cnf_data_ptr->search_cnf.access.access_method;

  if (rsp_ptr->rsp_payload.response.session_search_rsp.access_method ==
       (uint32) MMGSDI_EF_ENUM_ACCESS)
  {
    rsp_ptr->rsp_payload.response.session_search_rsp.file_enum  =
      (uint32)uimdiag_util_convert_from_mmgsdi_items_enum(
        cnf_data_ptr->search_cnf.access.file.file_enum);
  }

  if(rsp_ptr->rsp_payload.response.session_search_rsp.access_method  ==
      (uint32) MMGSDI_DF_ENUM_ACCESS)
  {
    rsp_ptr->rsp_payload.response.session_search_rsp.df_enum  =
      (uint32)cnf_data_ptr->search_cnf.access.file.df_enum;
  }

  if(rsp_ptr->rsp_payload.response.session_search_rsp.access_method ==
     (uint32) MMGSDI_BY_PATH_ACCESS)
  {
    rsp_ptr->rsp_payload.response.session_search_rsp.path_type_len  =
      int32touint32(cnf_data_ptr->search_cnf.access.file.path_type.path_len);

    if(cnf_data_ptr->search_cnf.access.file.path_type.path_len <=
       MMGSDI_MAX_PATH_LEN)
    {
      memscpy((void *)rsp_ptr->rsp_payload.response.session_search_rsp.path_type_ptr,
        sizeof(rsp_ptr->rsp_payload.response.session_search_rsp.path_type_ptr),
        cnf_data_ptr->search_cnf.access.file.path_type.path_buf,
        int32touint32(cnf_data_ptr->search_cnf.access.file.path_type.path_len));
    }
  }

  if(rsp_ptr->rsp_payload.response.session_search_rsp.access_method ==
    (uint32) MMGSDI_BY_APP_ID_ACCESS)
  {
    rsp_ptr->rsp_payload.response.session_search_rsp.app_id_len =
    int32touint32(cnf_data_ptr->search_cnf.access.file.app_id.data_len);

    if(cnf_data_ptr->search_cnf.access.file.app_id.data_len <=
       MMGSDI_MAX_AID_LEN)
    {
      memscpy((void *)rsp_ptr->rsp_payload.response.session_search_rsp.app_id_ptr,
        sizeof(rsp_ptr->rsp_payload.response.session_search_rsp.app_id_ptr),
        cnf_data_ptr->search_cnf.access.file.app_id.data_ptr,
        int32touint32(cnf_data_ptr->search_cnf.access.file.app_id.data_len));
    }
  }

  rsp_ptr->rsp_payload.response.session_search_rsp.searched_record_nums_len =
    int32touint32(cnf_data_ptr->search_cnf.searched_record_nums.data_len);

  if(cnf_data_ptr->search_cnf.searched_record_nums.data_len <=
     UIMDIAG_MAX_DATA_BLOCK_LEN)
  {
    memscpy((void *)rsp_ptr->rsp_payload.response.session_search_rsp.searched_record_nums_ptr,
      sizeof(rsp_ptr->rsp_payload.response.session_search_rsp.searched_record_nums_ptr),
      cnf_data_ptr->search_cnf.searched_record_nums.data_ptr,
      int32touint32(cnf_data_ptr->search_cnf.searched_record_nums.data_len));
  }

  return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_session_search_cnf */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_PROV_APP_INIT_CMPL_CNF

DESCRIPTION
  Function to populate the response pointer with the confirmation data

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type.

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_prov_app_init_cmpl_cnf(
  uimdiag_generic_rsp_type      *rsp_ptr,
  const mmgsdi_cnf_type         *cnf_data_ptr
)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cnf_data_ptr, rsp_ptr);

  (void)memset(
    (void*)&rsp_ptr->rsp_payload.response.session_prov_app_init_complete_rsp,
      0x00, sizeof(uimdiag_mmgsdi_sessionid_type));

  rsp_ptr->rsp_payload.response.session_prov_app_init_complete_rsp.session_id_low =
    UIMDIAG_LODWORD(
      (cnf_data_ptr->session_prov_app_init_complete_cnf.response_header.session_id));

  rsp_ptr->rsp_payload.response.session_prov_app_init_complete_rsp.session_id_high =
    UIMDIAG_HIDWORD(
      (cnf_data_ptr->session_prov_app_init_complete_cnf.response_header.session_id));
  return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_prov_app_init_cmpl_cnf */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_COMPUTE_IP_AUTH_CNF

DESCRIPTION
  Function to populate the response pointer with the conformation data

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type.

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_comp_ip_auth_cnf(
  uimdiag_generic_rsp_type      *rsp_ptr,
  const mmgsdi_cnf_type         *cnf_data_ptr
)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cnf_data_ptr, rsp_ptr);

  (void)memset(
    (void*)&rsp_ptr->rsp_payload.response.session_compute_ip_auth_rsp,
      0x00, sizeof(uimdiag_mmgsdi_session_compute_ip_auth_rsp_type));

  rsp_ptr->rsp_payload.response.session_compute_ip_auth_rsp.cmpt_ip_rsp_data_len =
    (uint32)cnf_data_ptr->compute_ip_cnf.cmpt_ip_response_data.data_len;

  memscpy((rsp_ptr->rsp_payload.response.session_compute_ip_auth_rsp.cmpt_ip_rsp_data),
    sizeof(rsp_ptr->rsp_payload.response.session_compute_ip_auth_rsp.cmpt_ip_rsp_data),
    (uint8*)cnf_data_ptr->compute_ip_cnf.cmpt_ip_response_data.data_ptr,
    MIN((uint32)cnf_data_ptr->compute_ip_cnf.cmpt_ip_response_data.data_len,
         UIM_COMPUTE_IP_AUTH_RESPONSE_LENGTH));

  return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_comp_ip_auth_cnf */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SSD_UPDATE_CNF

DESCRIPTION
  Function to populate the response pointer with the conformation data

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type.

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_ssd_update_cnf(
  uimdiag_generic_rsp_type      *rsp_ptr,
  const mmgsdi_cnf_type         *cnf_data_ptr
)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cnf_data_ptr, rsp_ptr);

  (void)memset(
    (void*)&rsp_ptr->rsp_payload.response.session_cnf_ssd_rsp,
      0x00, sizeof(uimdiag_mmgsdi_session_confirm_ssd_rsp_type));

  rsp_ptr->rsp_payload.response.session_ssd_update_rsp.session_id_low =
    UIMDIAG_LODWORD(
      (cnf_data_ptr->session_ssd_update_cnf.response_header.session_id));

  rsp_ptr->rsp_payload.response.session_ssd_update_rsp.session_id_high =
    UIMDIAG_HIDWORD(
      (cnf_data_ptr->session_ssd_update_cnf.response_header.session_id));
  return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_ssd_update_cnf */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SSD_CONFIRM_CNF

DESCRIPTION
  Function to populate the response pointer with the conformation data

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type.

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_ssd_confirm_cnf(
  uimdiag_generic_rsp_type      *rsp_ptr,
  const mmgsdi_cnf_type         *cnf_data_ptr
)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cnf_data_ptr, rsp_ptr);

  (void)memset(
    (void*)&rsp_ptr->rsp_payload.response.session_cnf_ssd_rsp,
      0x00, sizeof(uimdiag_mmgsdi_session_confirm_ssd_rsp_type));

  rsp_ptr->rsp_payload.response.session_cnf_ssd_rsp.ssd_update_ok =
    (uint32)cnf_data_ptr->session_ssd_confirm_cnf.ssd_update_ok;

  return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_ssd_confirm_cnf */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_BS_CHAL_CNF

DESCRIPTION
  Function to populate the response pointer with the conformation data

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type.

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_bs_chal_cnf(
  uimdiag_generic_rsp_type      *rsp_ptr,
  const mmgsdi_cnf_type         *cnf_data_ptr
)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cnf_data_ptr, rsp_ptr);

  (void)memset(
    (void*)&rsp_ptr->rsp_payload.response.session_bs_chal_rsp,
      0x00, sizeof(uimdiag_mmgsdi_session_bs_chal_rsp_type));

  rsp_ptr->rsp_payload.response.session_bs_chal_rsp.randbs =
    (uint32)cnf_data_ptr->session_bs_chal_cnf.randbs;

  return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_bs_chal_cnf */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_READ_PRL_CNF

DESCRIPTION
  Function to populate the response pointer with the conformation data

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type.

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_read_prl_cnf(
  uimdiag_generic_rsp_type      *rsp_ptr,
  const mmgsdi_cnf_type         *cnf_data_ptr
)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cnf_data_ptr, rsp_ptr);

  (void)memset(
    (void*)&rsp_ptr->rsp_payload.response.session_read_prl_rsp,
      0x00, sizeof(uimdiag_mmgsdi_session_read_prl_rsp_type));

  rsp_ptr->rsp_payload.response.session_read_prl_rsp.prl_version =
    (uint32)cnf_data_ptr->session_read_prl_cnf.prl_version;

  rsp_ptr->rsp_payload.response.session_read_prl_rsp.size =
    (uint32)cnf_data_ptr->session_read_prl_cnf.size;

  rsp_ptr->rsp_payload.response.session_read_prl_rsp.valid =
    (uint32)cnf_data_ptr->session_read_prl_cnf.valid;

  memscpy((rsp_ptr->rsp_payload.response.session_read_prl_rsp.roaming_list),
    sizeof(rsp_ptr->rsp_payload.response.session_read_prl_rsp.roaming_list),
    (uint8*)cnf_data_ptr->session_read_prl_cnf.roaming_list_ptr,
    MIN((uint32)cnf_data_ptr->session_read_prl_cnf.size,
    UIMDIAG_MAX_ROAMING_LIST_LENGTH));

  return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_read_prl_cnf */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_GET_INFO_SYNC_CNF

DESCRIPTION
  Function to populate the response pointer with the conformation data

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type.

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_session_get_info_sync_cnf(
  uimdiag_generic_rsp_type      *rsp_ptr,
  const mmgsdi_session_get_info_type         *session_get_info_cnf_ptr
)
{
    UIMDIAG_UTIL_RETURN_IF_NULL_2(rsp_ptr, session_get_info_cnf_ptr);

    (void)memset((void*)&rsp_ptr->rsp_payload.response.session_get_info_sync_rsp,
      0x00,sizeof(uimdiag_mmgsdi_rsp_session_get_info_pyld_type));

    rsp_ptr->rsp_payload.response.session_get_info_sync_rsp.session_type    =
                       (uint32) session_get_info_cnf_ptr->session_type;

    rsp_ptr->rsp_payload.response.session_get_info_sync_rsp.slot_id         =
      (uint32)session_get_info_cnf_ptr->slot_id;

    rsp_ptr->rsp_payload.response.session_get_info_sync_rsp.channel_id      =
         (uint32)session_get_info_cnf_ptr->channel_id;

    rsp_ptr->rsp_payload.response.session_get_info_sync_rsp.app_type        =
      (uint32)session_get_info_cnf_ptr->app_data.app_type;

    UIMDIAG_UTIL_RETURN_IF_OUT_OF_RANGE(
      session_get_info_cnf_ptr->app_data.aid.data_len,
      0,
      MMGSDI_MAX_AID_LEN);

    rsp_ptr->rsp_payload.response.session_get_info_sync_rsp.aid_length =
      session_get_info_cnf_ptr->app_data.aid.data_len;

    if(session_get_info_cnf_ptr->app_data.aid.data_len <= MMGSDI_MAX_AID_LEN)
      {
        memscpy((void *)(rsp_ptr->rsp_payload.response.session_get_info_sync_rsp.aid_ptr),
        sizeof(rsp_ptr->rsp_payload.response.session_get_info_sync_rsp.aid_ptr),
        session_get_info_cnf_ptr->app_data.aid.data_ptr,
        int32touint32(session_get_info_cnf_ptr->app_data.aid.data_len));
       }
     else
      {
        UIMDIAG_MSG_ERROR_0("Uimdiag Session Get info Sync  CNF: Invalid AID data ptr");
        return UIMDIAG_ERROR;
       }

     UIMDIAG_UTIL_RETURN_IF_OUT_OF_RANGE(
      session_get_info_cnf_ptr->app_data.label.data_len,
      0,
      MMGSDI_MAX_AID_LEN);

     rsp_ptr->rsp_payload.response.session_get_info_sync_rsp.label_len =
       session_get_info_cnf_ptr->app_data.label.data_len;

     if(session_get_info_cnf_ptr->app_data.label.data_len <=  MMGSDI_MAX_AID_LEN)
     {
       memscpy((void *)(rsp_ptr->rsp_payload.response.session_get_info.label_ptr),
       sizeof(rsp_ptr->rsp_payload.response.session_get_info.label_ptr),
       session_get_info_cnf_ptr->app_data.label.data_ptr,
       int32touint32(session_get_info_cnf_ptr->app_data.label.data_len));
     }
     else
     {
        UIMDIAG_MSG_ERROR_0("Uimdiag Session get info CNF: Invalid Label data ptr");
        return UIMDIAG_ERROR;
     }
     return UIMDIAG_SUCCESS;

} /* uimdiag_mmgsdi_session_get_info_sync_cnf */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_session_read_cache_cnf

DESCRIPTION
  Function to populate the response pointer with the conformation data

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type.

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_session_read_cache_cnf(
  uimdiag_generic_rsp_type      *rsp_ptr,
  const mmgsdi_data_type        *read_data_ptr
)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(read_data_ptr, rsp_ptr);

  (void)memset(
    (void*)&rsp_ptr->rsp_payload.response.session_read_cache_rsp,
      0x00, sizeof(uimdiag_mmgsdi_session_read_cache_rsp_type));

  rsp_ptr->rsp_payload.response.session_read_cache_rsp.read_data_len =
                                          read_data_ptr->data_len;

  memscpy(rsp_ptr->rsp_payload.response.session_read_cache_rsp.read_data,
          sizeof(rsp_ptr->rsp_payload.response.session_read_cache_rsp.read_data),
          read_data_ptr->data_ptr,
          MIN(read_data_ptr->data_len, UIMDIAG_MAX_READ_DATA_LEN));
  return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_session_read_cache_cnf */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_sap_get_atr_cnf

DESCRIPTION
  Function to populate the response pointer with the conformation data

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type.

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_sap_get_atr_cnf(
  uimdiag_generic_rsp_type           *rsp_ptr,
  const mmgsdi_cnf_type              *cnf_ptr
)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cnf_ptr, rsp_ptr);

  (void)memset((void*)&rsp_ptr->rsp_payload.response.sap_get_atr_rsp,
               0x00,sizeof(uimdiag_mmgsdi_sap_get_atr_rsp_type));

  rsp_ptr->rsp_payload.response.sap_get_atr_rsp.atr_data_len =
      cnf_ptr->sap_get_atr_cnf.atr_data.data_len;
  if(cnf_ptr->sap_get_atr_cnf.atr_data.data_len <= UIM_MAX_ATR_CHARS)
  {
    memscpy(rsp_ptr->rsp_payload.response.sap_get_atr_rsp.atr_data,
             sizeof(rsp_ptr->rsp_payload.response.sap_get_atr_rsp.atr_data),
             cnf_ptr->sap_get_atr_cnf.atr_data.data_ptr,
             cnf_ptr->sap_get_atr_cnf.atr_data.data_len);

    return UIMDIAG_SUCCESS;
  }
  else
  {
    UIMDIAG_MSG_HIGH_1("In uimdiag_mmgsdi_sap_get_atr_cnf, Length %d greater than max value",
                       cnf_ptr->sap_get_atr_cnf.atr_data.data_len);
    return UIMDIAG_ERROR;
  }
} /* uimdiag_mmgsdi_sap_get_atr_cnf */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_sap_send_apdu_cnf

DESCRIPTION
  Function to populate the response pointer with the conformation data

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type.

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_sap_send_apdu_cnf(
  uimdiag_generic_rsp_type           *rsp_ptr,
  const mmgsdi_cnf_type              *cnf_ptr
)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cnf_ptr, rsp_ptr);

  (void)memset((void*)&rsp_ptr->rsp_payload.response.sap_send_apdu_rsp,
               0x00,sizeof(uimdiag_mmgsdi_sap_send_apdu_rsp_type));

  rsp_ptr->rsp_payload.response.sap_send_apdu_rsp.max_size =
      cnf_ptr->sap_send_apdu_cnf.max_size;

  rsp_ptr->rsp_payload.response.sap_send_apdu_rsp.channel_id =
      cnf_ptr->sap_send_apdu_cnf.channel_id;

  rsp_ptr->rsp_payload.response.sap_send_apdu_rsp.implicit_get_rsp_sw1 =
      cnf_ptr->sap_send_apdu_cnf.implicit_get_rsp_sw1;

  rsp_ptr->rsp_payload.response.sap_send_apdu_rsp.implicit_get_rsp_sw2 =
      cnf_ptr->sap_send_apdu_cnf.implicit_get_rsp_sw2;

  rsp_ptr->rsp_payload.response.sap_send_apdu_rsp.apdu_data_len =
      cnf_ptr->sap_send_apdu_cnf.apdu_data.data_len;

  if(cnf_ptr->sap_send_apdu_cnf.apdu_data.data_len <= UIMDIAG_MAX_DATA_BLOCK_LEN)
  {
    memscpy(rsp_ptr->rsp_payload.response.sap_send_apdu_rsp.apdu_data,
             sizeof(rsp_ptr->rsp_payload.response.sap_send_apdu_rsp.apdu_data),
             cnf_ptr->sap_send_apdu_cnf.apdu_data.data_ptr,
             cnf_ptr->sap_send_apdu_cnf.apdu_data.data_len);
    return UIMDIAG_SUCCESS;
  }
  else
  {
    UIMDIAG_MSG_HIGH_1("In uimdiag_mmgsdi_sap_send_apdu_cnf, Length %d greater than max value of 512",
                       cnf_ptr->sap_send_apdu_cnf.apdu_data.data_len);
    return UIMDIAG_ERROR;
  }
} /* uimdiag_mmgsdi_sap_send_apdu_cnf */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_sap_generic_cnf

DESCRIPTION
  Function to populate the response pointer with the conformation data

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type.

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_sap_generic_cnf(
  uimdiag_generic_rsp_type           *rsp_ptr,
  const mmgsdi_cnf_type              *cnf_ptr,
  mmgsdi_cnf_enum_type                cnf_type
)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cnf_ptr, rsp_ptr);

  switch(cnf_type)
  {
    case MMGSDI_SAP_POWER_ON_CNF:
      (void)memset((void*)&rsp_ptr->rsp_payload.response.sap_power_on_rsp,
                   0x00,sizeof(uimdiag_mmgsdi_sap_generic_rsp_type));
      rsp_ptr->rsp_payload.response.sap_power_on_rsp.client_id_low =
        UIMDIAG_LODWORD(cnf_ptr->sap_power_on_cnf.response_header.client_id);

      rsp_ptr->rsp_payload.response.sap_power_on_rsp.client_id_high =
        UIMDIAG_HIDWORD(cnf_ptr->sap_power_on_cnf.response_header.client_id);
      break;

    case MMGSDI_SAP_POWER_OFF_CNF:
      (void)memset((void*)&rsp_ptr->rsp_payload.response.sap_power_off_rsp,
                   0x00,sizeof(uimdiag_mmgsdi_sap_generic_rsp_type));
      rsp_ptr->rsp_payload.response.sap_power_off_rsp.client_id_low =
        UIMDIAG_LODWORD(cnf_ptr->sap_power_off_cnf.response_header.client_id);

      rsp_ptr->rsp_payload.response.sap_power_off_rsp.client_id_high =
        UIMDIAG_HIDWORD(cnf_ptr->sap_power_off_cnf.response_header.client_id);
      break;

    case MMGSDI_SAP_RESET_CNF:
      (void)memset((void*)&rsp_ptr->rsp_payload.response.sap_reset_rsp,
                   0x00,sizeof(uimdiag_mmgsdi_sap_generic_rsp_type));
      rsp_ptr->rsp_payload.response.sap_reset_rsp.client_id_low =
        UIMDIAG_LODWORD(cnf_ptr->sap_reset_cnf.response_header.client_id);

      rsp_ptr->rsp_payload.response.sap_reset_rsp.client_id_high =
        UIMDIAG_HIDWORD(cnf_ptr->sap_reset_cnf.response_header.client_id);
      break;

    case MMGSDI_SAP_CONNECT_CNF:
      (void)memset((void*)&rsp_ptr->rsp_payload.response.sap_connect_rsp,
                   0x00,sizeof(uimdiag_mmgsdi_sap_generic_rsp_type));
      rsp_ptr->rsp_payload.response.sap_connect_rsp.client_id_low =
        UIMDIAG_LODWORD(cnf_ptr->sap_connect_cnf.response_header.client_id);

      rsp_ptr->rsp_payload.response.sap_connect_rsp.client_id_high =
        UIMDIAG_HIDWORD(cnf_ptr->sap_connect_cnf.response_header.client_id);
      break;

    default:
      UIMDIAG_MSG_ERROR_1("Unexpected confiramtion type: %x ", cnf_type);
      return UIMDIAG_ERROR;
  }
  return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_sap_generic_cnf */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_sap_card_reader_status_cnf

DESCRIPTION
  Function to populate the response pointer with the conformation data

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type.

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_sap_card_reader_status_cnf(
  uimdiag_generic_rsp_type           *rsp_ptr,
  const mmgsdi_cnf_type              *cnf_ptr
)
{
  uimdiag_return_enum_type              uimdiag_status         = UIMDIAG_ERROR;
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cnf_ptr, rsp_ptr);

  (void)memset((void*)&rsp_ptr->rsp_payload.response.sap_card_reader_status_rsp,
               0x00,sizeof(uimdiag_mmgsdi_sap_card_reader_status_rsp_type));

  UIMDIAG_UTIL_TMC_MEM_MALLOC_AND_VALIDATE(
    rsp_ptr->rsp_payload.response.sap_card_reader_status_rsp.data_ptr,
      (sizeof(uint8)*cnf_ptr->sap_card_reader_status_cnf.card_reader_status_data.data_len));

  if(rsp_ptr->rsp_payload.response.sap_card_reader_status_rsp.data_ptr == NULL)
  {
     return UIMDIAG_ERROR;
  }

  rsp_ptr->rsp_payload.response.sap_card_reader_status_rsp.data_len =
      cnf_ptr->sap_card_reader_status_cnf.card_reader_status_data.data_len;

  memscpy(rsp_ptr->rsp_payload.response.sap_card_reader_status_rsp.data_ptr,
         sizeof(rsp_ptr->rsp_payload.response.sap_card_reader_status_rsp.data_ptr),
         cnf_ptr->sap_card_reader_status_cnf.card_reader_status_data.data_ptr,
         cnf_ptr->sap_card_reader_status_cnf.card_reader_status_data.data_len);

  return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_sap_card_reader_status_cnf */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_get_operator_name_cnf

DESCRIPTION
  Function to populate the response pointer with the conformation data

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type.

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_get_operator_name_cnf(
  uimdiag_generic_rsp_type           *rsp_ptr,
  const mmgsdi_cnf_type              *cnf_ptr
)
{
  uimdiag_mmgsdi_rsp_session_get_operator_name_type *get_operator_name_rsp_ptr = NULL;
  uint8 *offsetting_rsp_ptr = NULL;
  uint32 index = 0;
  uint32 total_len = 0;
  uint8 *temp_total_name_len_ptr = NULL;
  uint8  temp_data = 0;

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cnf_ptr, rsp_ptr);

   /***
         struct of response
         uint8    encoding_info;
         uint32   operator_name_len;
         uint8    operator_name[1];
         uint8    lang_ind;
         uint8    spn_display_bit;
         uint8    rplmn_display_bit;
         uint8    roaming_status;
         uint32   no_plmn_id;
         uint32   total_buff_size_of_operator_name_info;

         Dynamic part encoding
         uint8    plmn_id[3]
         uint8    rat_type
         uint8    long_name_ci;
         uint8    long_name_spare_bytes;
         uint8    long_name_encoding;
         uint32   long name len
         uint8    *long name
         uint8    short_name_ci;
         uint8    short_name_spare_bytes;
         uint8    short_name_encoding;
         uint32   short name len
         uint8    *short name
      */

  (void)memset((void*)&rsp_ptr->rsp_payload.response.session_get_operator_name,
               0x00,sizeof(uimdiag_mmgsdi_rsp_session_get_operator_name_type));

  get_operator_name_rsp_ptr = &rsp_ptr->rsp_payload.response.session_get_operator_name;

  get_operator_name_rsp_ptr->encoding_info =  cnf_ptr->session_get_operator_name_cnf.spn.eons_encoding;
  get_operator_name_rsp_ptr->operator_name_len =  cnf_ptr->session_get_operator_name_cnf.spn.eons_data.data_len;
  offsetting_rsp_ptr = (uint8 *)(get_operator_name_rsp_ptr->operator_name);

  /* operator name */
  if (cnf_ptr->session_get_operator_name_cnf.spn.eons_data.data_len > 0)
  {
    memscpy((void*)offsetting_rsp_ptr, cnf_ptr->session_get_operator_name_cnf.spn.eons_data.data_len,
                  (void *)cnf_ptr->session_get_operator_name_cnf.spn.eons_data.data_ptr,
                   cnf_ptr->session_get_operator_name_cnf.spn.eons_data.data_len);
    offsetting_rsp_ptr += cnf_ptr->session_get_operator_name_cnf.spn.eons_data.data_len;
  }

  /* Language indicator */
  memscpy((void *)offsetting_rsp_ptr, sizeof(uint8), (void *)&cnf_ptr->session_get_operator_name_cnf.spn_lang_ind,
                 sizeof(uint8));
  offsetting_rsp_ptr += sizeof(uint8);

  /* SPN display bit */
  memscpy((void *)offsetting_rsp_ptr, sizeof(uint8), (void *)&cnf_ptr->session_get_operator_name_cnf.spn_display_bit,
                 sizeof(uint8));
  offsetting_rsp_ptr += sizeof(uint8);

  /* RPLMN display bit */
  memscpy((void *)offsetting_rsp_ptr, sizeof(uint8), (void *)&cnf_ptr->session_get_operator_name_cnf.rplmn_display_bit,
                 sizeof(uint8));
  offsetting_rsp_ptr += sizeof(uint8);

  /* Roaming status */
  memscpy((void *)offsetting_rsp_ptr, sizeof(uint8), (void *)&cnf_ptr->session_get_operator_name_cnf.roaming_status,
                 sizeof(uint8));
  offsetting_rsp_ptr += sizeof(uint8);

  /* no of plmn ids name */
  memscpy((void *)offsetting_rsp_ptr, sizeof(uint32), (void *)&cnf_ptr->session_get_operator_name_cnf.plmn_list.num_of_plmn_ids,
                 sizeof(uint32));
  offsetting_rsp_ptr += sizeof(uint32);

  temp_total_name_len_ptr = offsetting_rsp_ptr;
  /* for now skip 4 bytes for storing total operator name info len */
  /* this will be copied at the end */
  offsetting_rsp_ptr += sizeof(uint32);

  for(index =0; index < cnf_ptr->session_get_operator_name_cnf.plmn_list.num_of_plmn_ids; index ++)
  {
    /* PLMN ID */
    memscpy((void *)offsetting_rsp_ptr,
      sizeof(uint8) * 3,
      (void *)cnf_ptr->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[index].plmn_id.plmn_id_val,
      sizeof(uint8) * 3);
    offsetting_rsp_ptr += sizeof(uint8) * 3;
    total_len += sizeof(uint8) * 3;

    /* RAT Type */

    /* copy the enum value into uint8 variable and then do memscpy to make it
       endian independent */
    temp_data =
      cnf_ptr->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[index].plmn_id.rat;

    memscpy((void *)offsetting_rsp_ptr,
      sizeof(uint8),
      (void *)&temp_data,
      sizeof(uint8));
    offsetting_rsp_ptr += sizeof(uint8);
    total_len += sizeof(uint8);

    /* Long name CI */
    memscpy((void *)offsetting_rsp_ptr,
      sizeof(uint8),
      (void *)&cnf_ptr->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[index].plmn_long_name.plmn_name_ci,
      sizeof(uint8));
    offsetting_rsp_ptr += sizeof(uint8);
    total_len += sizeof(uint8);

    /*Long name Spare Bits */
    memscpy((void *)offsetting_rsp_ptr,
      sizeof(uint8),
      (void *)&cnf_ptr->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[index].plmn_long_name.plmn_name_spare_bits,
      sizeof(uint8));
    offsetting_rsp_ptr += sizeof(uint8);
    total_len += sizeof(uint8);

    /* Long name encoding */

    /* copy the enum value into uint8 variable and then do memscpy to make it
       endian independent */
    temp_data =
      cnf_ptr->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[index].plmn_long_name.plmn_name.eons_encoding;

    memscpy((void *)offsetting_rsp_ptr,
      sizeof(uint8),
      (void *)&temp_data,
      sizeof(uint8));
    offsetting_rsp_ptr += sizeof(uint8);
    total_len += sizeof(uint8);

    /* Long name len */
    memscpy((void *)offsetting_rsp_ptr,
      sizeof(uint32),
      (void *)&cnf_ptr->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[index].plmn_long_name.plmn_name.eons_data.data_len,
      sizeof(uint32));
    offsetting_rsp_ptr += sizeof(uint32);
    total_len += sizeof(uint32);

    /* Long name */
    memscpy((void *)offsetting_rsp_ptr,
      cnf_ptr->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[index].plmn_long_name.plmn_name.eons_data.data_len,
      (void *)cnf_ptr->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[index].plmn_long_name.plmn_name.eons_data.data_ptr,
      cnf_ptr->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[index].plmn_long_name.plmn_name.eons_data.data_len);
    offsetting_rsp_ptr += cnf_ptr->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[index].plmn_long_name.plmn_name.eons_data.data_len;
    total_len += cnf_ptr->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[index].plmn_long_name.plmn_name.eons_data.data_len;

    /* Short name CI */
    memscpy((void *)offsetting_rsp_ptr,
      sizeof(uint8),
      (void *)&cnf_ptr->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[index].plmn_short_name.plmn_name_ci,
      sizeof(uint8));
    offsetting_rsp_ptr += sizeof(uint8);
    total_len += sizeof(uint8);

    /*Short name Spare Bits */
    memscpy((void *)offsetting_rsp_ptr,
      sizeof(uint8),
      (void *)&cnf_ptr->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[index].plmn_short_name.plmn_name_spare_bits,
      sizeof(uint8));
    offsetting_rsp_ptr += sizeof(uint8);
    total_len += sizeof(uint8);

    /* Short name encoding */

    /* copy the enum value into uint8 variable and then do memscpy to make it
       endian independent */
    temp_data =
      cnf_ptr->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[index].plmn_short_name.plmn_name.eons_encoding;

    memscpy((void *)offsetting_rsp_ptr,
      sizeof(uint8),
      (void *)&temp_data,
      sizeof(uint8));
    offsetting_rsp_ptr += sizeof(uint8);
    total_len += sizeof(uint8);

    /* Short name len */
    memscpy((void *)offsetting_rsp_ptr,
      sizeof(uint32),
      (void *)&cnf_ptr->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[index].plmn_short_name.plmn_name.eons_data.data_len,
      sizeof(uint32));
    offsetting_rsp_ptr += sizeof(uint32);
    total_len += sizeof(uint32);


    /* Short name */
    memscpy((void *)offsetting_rsp_ptr,
      cnf_ptr->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[index].plmn_short_name.plmn_name.eons_data.data_len,
      (void *)cnf_ptr->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[index].plmn_short_name.plmn_name.eons_data.data_ptr,
      cnf_ptr->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[index].plmn_short_name.plmn_name.eons_data.data_len);
    offsetting_rsp_ptr += cnf_ptr->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[index].plmn_short_name.plmn_name.eons_data.data_len;
    total_len +=  cnf_ptr->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[index].plmn_short_name.plmn_name.eons_data.data_len;

  }

  memscpy((void *)temp_total_name_len_ptr, sizeof(uint32), (void *)&total_len, sizeof(uint32));

  return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_get_operator_name_cnf */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_sap_disconnect_cnf

DESCRIPTION
  Function to populate the response pointer with the conformation data

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type.

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_sap_disconnect_cnf(
  uimdiag_generic_rsp_type           *rsp_ptr,
  const mmgsdi_cnf_type              *cnf_ptr
)
{
  UIMDIAG_UTIL_RETURN_IF_NULL(cnf_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(rsp_ptr);

  (void)memset((void*)&rsp_ptr->rsp_payload.response.sap_disconnect_rsp,
               0x00,sizeof(uimdiag_mmgsdi_sap_disconnect_rsp_type));

  rsp_ptr->rsp_payload.response.sap_disconnect_rsp.sap_mode =
      cnf_ptr->sap_disconnect_cnf.disconnect_mode;

  return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_sap_disconnect_cnf */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_card_pdown_cnf

DESCRIPTION
  Function to populate the response pointer with the conformation data

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type.

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_card_pdown_cnf(
  uimdiag_generic_rsp_type           *rsp_ptr,
  const mmgsdi_cnf_type              *cnf_ptr
)
{
  UIMDIAG_UTIL_RETURN_IF_NULL(cnf_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(rsp_ptr);

  (void)memset((void*)&rsp_ptr->rsp_payload.response.card_pdown_rsp,
               0x00,sizeof(uimdiag_mmgsdi_card_pdown_rsp_type));

  rsp_ptr->rsp_payload.response.card_pdown_rsp.power_down_option =
      cnf_ptr->card_pdown_cnf.option;

  return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_card_pdown_cnf */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_card_pup_cnf

DESCRIPTION
  Function to populate the response pointer with the conformation data

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type.

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_card_pup_cnf(
  uimdiag_generic_rsp_type           *rsp_ptr,
  const mmgsdi_cnf_type              *cnf_ptr
)
{
  UIMDIAG_UTIL_RETURN_IF_NULL(cnf_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(rsp_ptr);

  (void)memset((void*)&rsp_ptr->rsp_payload.response.card_pup_rsp,
               0x00,sizeof(uimdiag_mmgsdi_card_pup_rsp_type));

  rsp_ptr->rsp_payload.response.card_pup_rsp.power_up_option =
      cnf_ptr->card_pup_cnf.option;

  return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_card_pup_cnf */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_activate_onchip_sim_cnf

DESCRIPTION
  Function to populate the response pointer with the conformation data

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type.

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_activate_onchip_sim_cnf(
  uimdiag_generic_rsp_type           *rsp_ptr,
  const mmgsdi_cnf_type              *cnf_ptr
)
{
  UIMDIAG_UTIL_RETURN_IF_NULL(cnf_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(rsp_ptr);

  (void)memset((void*)&rsp_ptr->rsp_payload.response.activate_onchip_sim_rsp,
               0x00,sizeof(uimdiag_mmgsdi_activate_onchip_sim_rsp_type));

  rsp_ptr->rsp_payload.response.activate_onchip_sim_rsp.onchip_state =
      cnf_ptr->act_onchip_cnf.onchip_sim_state.state;

  return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_activate_onchip_sim_cnf */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_send_apdu_cnf

DESCRIPTION
  Function to populate the response pointer with the conformation data

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type.

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_send_apdu_cnf(
  uimdiag_generic_rsp_type           *rsp_ptr,
  const mmgsdi_cnf_type              *cnf_ptr
)
{
  UIMDIAG_UTIL_RETURN_IF_NULL(cnf_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(rsp_ptr);

  (void)memset((void*)&rsp_ptr->rsp_payload.response.send_apdu_rsp,
               0x00,sizeof(uimdiag_mmgsdi_send_apdu_rsp_type));

  rsp_ptr->rsp_payload.response.send_apdu_rsp.max_size =
      cnf_ptr->send_apdu_cnf.max_size;
  rsp_ptr->rsp_payload.response.send_apdu_rsp.channel_id =
      cnf_ptr->send_apdu_cnf.channel_id;
  rsp_ptr->rsp_payload.response.send_apdu_rsp.implicit_sw1 =
      cnf_ptr->send_apdu_cnf.implicit_get_rsp_sw1;
  rsp_ptr->rsp_payload.response.send_apdu_rsp.implicit_sw2 =
      cnf_ptr->send_apdu_cnf.implicit_get_rsp_sw2;
  rsp_ptr->rsp_payload.response.send_apdu_rsp.apdu_data_length =
      cnf_ptr->send_apdu_cnf.apdu_data.data_len;
  if(cnf_ptr->send_apdu_cnf.apdu_data.data_len > UIMDIAG_MAX_DATA_BLOCK_LEN)
  {
    UIMDIAG_MSG_ERROR_0("Rsp len is greater than expected value.. rsp data will be truncated");
  }
  memscpy((void*)rsp_ptr->rsp_payload.response.send_apdu_rsp.apdu_data_ptr,
         sizeof(rsp_ptr->rsp_payload.response.send_apdu_rsp.apdu_data_ptr),
         (void*)cnf_ptr->send_apdu_cnf.apdu_data.data_ptr,
         MIN(cnf_ptr->send_apdu_cnf.apdu_data.data_len, UIMDIAG_MAX_DATA_BLOCK_LEN));

  return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_send_apdu_cnf */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_COPY_APP_INFO

DESCRIPTION
  This function is called to copy app information to rsp_ptr

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void uimdiag_mmgsdi_copy_app_info(
  uimdiag_mmgsdi_app_info_type   *temp_aid_info_ptr,
  const mmgsdi_app_info_type     *temp_aid_cnf_data_ptr,
  uint32                          aid_info_index,
  uint32                          no_of_apps
)
{
  uint32 aid_cnf_index = 0;

  if((temp_aid_info_ptr != NULL) &&
     (temp_aid_cnf_data_ptr != NULL))
  {

    for( ; aid_cnf_index < no_of_apps;
         aid_info_index++, aid_cnf_index++)
    {
      temp_aid_info_ptr[aid_info_index].cached_status =
           temp_aid_cnf_data_ptr[aid_cnf_index].cached;

      temp_aid_info_ptr[aid_info_index].slot_id =
           temp_aid_cnf_data_ptr[aid_cnf_index].slot;

      temp_aid_info_ptr[aid_info_index].pin1_id =
           temp_aid_cnf_data_ptr[aid_cnf_index].pin1.pin_id;

      temp_aid_info_ptr[aid_info_index].pin1_status =
           temp_aid_cnf_data_ptr[aid_cnf_index].pin1.status;

      temp_aid_info_ptr[aid_info_index].pin1_replacement_status =
           temp_aid_cnf_data_ptr[aid_cnf_index].pin1.pin_replacement;

      temp_aid_info_ptr[aid_info_index].pin1_num_of_retries =
           temp_aid_cnf_data_ptr[aid_cnf_index].pin1.num_retries;

      temp_aid_info_ptr[aid_info_index].pin1_num_of_unblock_retires =
           temp_aid_cnf_data_ptr[aid_cnf_index].pin1.num_unblock_retries;

      temp_aid_info_ptr[aid_info_index].pin2_id =
           temp_aid_cnf_data_ptr[aid_cnf_index].pin2.pin_id;

      temp_aid_info_ptr[aid_info_index].pin2_status =
           temp_aid_cnf_data_ptr[aid_cnf_index].pin2.status;

      temp_aid_info_ptr[aid_info_index].pin2_replacement_status =
           temp_aid_cnf_data_ptr[aid_cnf_index].pin2.pin_replacement;

      temp_aid_info_ptr[aid_info_index].pin2_num_of_retries =
           temp_aid_cnf_data_ptr[aid_cnf_index].pin2.num_retries;

      temp_aid_info_ptr[aid_info_index].pin2_num_of_unblock_retires =
           temp_aid_cnf_data_ptr[aid_cnf_index].pin2.num_unblock_retries;

      temp_aid_info_ptr[aid_info_index].upin_id =
           temp_aid_cnf_data_ptr[aid_cnf_index].universal_pin.pin_id;

      temp_aid_info_ptr[aid_info_index].upin_status =
           temp_aid_cnf_data_ptr[aid_cnf_index].universal_pin.status;

      temp_aid_info_ptr[aid_info_index].upin_replacement_status =
           temp_aid_cnf_data_ptr[aid_cnf_index].universal_pin.pin_replacement;

      temp_aid_info_ptr[aid_info_index].upin_num_of_retries =
           temp_aid_cnf_data_ptr[aid_cnf_index].universal_pin.num_retries;

      temp_aid_info_ptr[aid_info_index].upin_num_of_unblock_retires =
           temp_aid_cnf_data_ptr[aid_cnf_index].universal_pin.num_unblock_retries;

      temp_aid_info_ptr[aid_info_index].app_type  =
           temp_aid_cnf_data_ptr[aid_cnf_index].app_data.app_type;

      temp_aid_info_ptr[aid_info_index].aid_len =
           temp_aid_cnf_data_ptr[aid_cnf_index].app_data.aid.data_len;

      memscpy((void *) temp_aid_info_ptr[aid_info_index].app_id_ptr,
              sizeof(temp_aid_info_ptr[aid_info_index].app_id_ptr),
             (void *) temp_aid_cnf_data_ptr[aid_cnf_index].app_data.aid.data_ptr,
              temp_aid_cnf_data_ptr[aid_cnf_index].app_data.aid.data_len);

      temp_aid_info_ptr[aid_info_index].label_len =
           temp_aid_cnf_data_ptr[aid_cnf_index].app_data.label.data_len;

      memscpy((void *) temp_aid_info_ptr[aid_info_index].label_ptr,
             sizeof(temp_aid_info_ptr[aid_info_index].label_ptr),
             (void *) temp_aid_cnf_data_ptr[aid_cnf_index].app_data.label.data_ptr,
             temp_aid_cnf_data_ptr[aid_cnf_index].app_data.label.data_len);

      temp_aid_info_ptr[aid_info_index].hkey_id =
           temp_aid_cnf_data_ptr[aid_cnf_index].hiddenkey.pin_id;

      temp_aid_info_ptr[aid_info_index].hkey_status =
           temp_aid_cnf_data_ptr[aid_cnf_index].hiddenkey.status;

      temp_aid_info_ptr[aid_info_index].hkey_replacement_status =
           temp_aid_cnf_data_ptr[aid_cnf_index].hiddenkey.pin_replacement;

      temp_aid_info_ptr[aid_info_index].hkey_num_of_retries =
           temp_aid_cnf_data_ptr[aid_cnf_index].hiddenkey.num_retries;

      temp_aid_info_ptr[aid_info_index].hkey_num_of_unblock_retires =
           temp_aid_cnf_data_ptr[aid_cnf_index].hiddenkey.num_unblock_retries;

      temp_aid_info_ptr[aid_info_index].dir_index =
           temp_aid_cnf_data_ptr[aid_cnf_index].dir_index;
    }
  }
}/* uimdiag_mmgsdi_copy_app_info */


/*===========================================================================

FUNCTION uimdiag_mmgsdi_get_all_available_apps_cnf

DESCRIPTION
  Function to populate the response pointer with the conformation data

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type.

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_get_all_available_apps_cnf(
  uimdiag_generic_rsp_type          *rsp_ptr,
  const mmgsdi_available_apps_type  *apps_info_ptr
)
{
  int32                         total_aids            = 0;
  uimdiag_mmgsdi_app_info_type *temp_aid_info_ptr     = NULL;
  const mmgsdi_app_info_type   *temp_aid_cnf_data_ptr = NULL;

  UIMDIAG_UTIL_RETURN_IF_NULL(apps_info_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(rsp_ptr);

  total_aids = apps_info_ptr->card1_num_app + apps_info_ptr->card2_num_app +
               apps_info_ptr->card3_num_app;

  temp_aid_info_ptr =  rsp_ptr->rsp_payload.response.get_all_available_apps_rsp.get_all_available_app_info;
  temp_aid_cnf_data_ptr = apps_info_ptr->card1_app_info;

  /* Copy app info from card1 */

  (void)uimdiag_mmgsdi_copy_app_info(temp_aid_info_ptr,
                                     temp_aid_cnf_data_ptr,
                                     0,
                                     (uint32) apps_info_ptr->card1_num_app);

   temp_aid_cnf_data_ptr = apps_info_ptr->card2_app_info;

   /* Copy app info from card2 */

  (void)uimdiag_mmgsdi_copy_app_info(temp_aid_info_ptr,
                                     temp_aid_cnf_data_ptr,
                                     (uint32) apps_info_ptr->card1_num_app,
                                     (uint32) apps_info_ptr->card2_num_app);
   /* Copy app info from card3 */

  temp_aid_cnf_data_ptr = apps_info_ptr->card3_app_info;

  (void)uimdiag_mmgsdi_copy_app_info(temp_aid_info_ptr,
                                     temp_aid_cnf_data_ptr,
                                    (uint32)(apps_info_ptr->card1_num_app +
                                     apps_info_ptr->card2_num_app),
                                    (uint32) apps_info_ptr->card3_num_app);

  rsp_ptr->rsp_payload.response.get_all_available_apps_rsp.total_no_of_apps =
    total_aids;

  return UIMDIAG_SUCCESS;
}/* uimdiag_mmgsdi_get_all_available_apps_cnf */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_PROC_SESSION_OPEN_EXT_CNF

DESCRIPTION
  Function to populate the response pointer with the confirmation data

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_proc_session_open_ext_cnf(
  uimdiag_generic_rsp_type      *rsp_ptr,
  const mmgsdi_cnf_type         *cnf_data_ptr,
  uint32                         rsp_len
)
{

  uint32                         session_info_len = 0;

  UIMDIAG_UTIL_RETURN_IF_NULL(cnf_data_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(rsp_ptr);

  if(cnf_data_ptr->session_open_ext_cnf.num_sessions > UIMDIAG_MAX_SESSIONS)
  {
    return UIMDIAG_ERROR;
  }

  if (rsp_len > (sizeof(uimdiag_cmd_rsp_hdr_type) +
                 sizeof(mmgsdi_return_enum_type)))
  {
    /* Calculating the actual length to store sessions information */
    session_info_len = rsp_len -
                         sizeof(uimdiag_cmd_rsp_hdr_type) -
                         sizeof(mmgsdi_return_enum_type);
  }
  else
  {
    return UIMDIAG_ERROR;
  }

  if (session_info_len > sizeof(uimdiag_mmgsdi_rsp_session_open_ext_pyld_type))
  {
    return UIMDIAG_ERROR;
  }

  (void)memset((void*)&rsp_ptr->rsp_payload.response.session_open_ext,
    0x00, session_info_len);

  rsp_ptr->rsp_payload.response.session_open_ext.num_sessions =
    (uint8)cnf_data_ptr->session_open_ext_cnf.num_sessions;

  memscpy((void*)rsp_ptr->rsp_payload.response.session_open_ext.session_info,
          session_info_len,
          cnf_data_ptr->session_open_ext_cnf.session_info_ptr,
          (uint32)(cnf_data_ptr->session_open_ext_cnf.num_sessions
          * sizeof(uimdiag_mmgsdi_session_open_info_type)));

  return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_proc_session_open_ext_cnf*/

/*===========================================================================

FUNCTION uimdiag_mmgsdi_session_get_nvruim_data_sync_cnf

DESCRIPTION
  Function to populate the response pointer with the conformation data

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type.

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_session_get_nvruim_data_sync_cnf(
  uimdiag_generic_rsp_type      *rsp_ptr,
  const mmgsdi_nvruim_data_type    *nvruim_data_ptr
)
{
  UIMDIAG_MSG_HIGH_0("In uimdiag_mmgsdi_session_get_nvruim_data_sync_cnf");
  UIMDIAG_UTIL_RETURN_IF_NULL(nvruim_data_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(rsp_ptr);

  (void)memset(
    (void*)&rsp_ptr->rsp_payload.response.session_get_nvruim_data_sync,
      0x00, sizeof(uimdiag_mmgsdi_session_get_nvruim_data_sync_rsp_type));

  rsp_ptr->rsp_payload.response.session_get_nvruim_data_sync.nvruim_data_req_enum =
    nvruim_data_ptr->req_type;

/*
switch(rsp_ptr->rsp_payload.response.session_get_nvruim_data_sync.nvruim_data_req_enum)
{

 case MMGSDI_NVRUIM_GET_ESN_USAGE_REQ:
    rsp_ptr->rsp_payload.response.session_get_nvruim_data_sync.esn_usage_enum =
        nvruim_data_ptr->data.esn_usage;
    break;

  case MMGSDI_NVRUIM_GET_HRPD_CONTROL_REQ:
    rsp_ptr->rsp_payload.response.session_get_nvruim_data_sync.an_hrpd_status_enum =
        nvruim_data_ptr->data.an_hrpd_status;
    break;

  case MMGSDI_NVRUIM_GET_3GPD_EXT_SUPPORT_REQ:
    rsp_ptr->rsp_payload.response.session_get_nvruim_data_sync.nvruim_3gpd_ext_support =
        nvruim_data_ptr->data.nvruim_3gpd_ext_support;
    break;

  case MMGSDI_NVRUIM_GET_3GPD_CTRL_AND_ESN_USG_DATA_REQ:
    rsp_ptr->rsp_payload.response.session_get_nvruim_data_sync.nvruim_3gpd_sip =
        nvruim_data_ptr->data.nvruim_3gpd_ctrl_and_esn_usg.mmgsdi_nvruim_3gpd_sip_svc;

    rsp_ptr->rsp_payload.response.session_get_nvruim_data_sync.nvruim_3gpd_mip =
        nvruim_data_ptr->data.nvruim_3gpd_ctrl_and_esn_usg.mmgsdi_nvruim_3gpd_mip_svc;

    rsp_ptr->rsp_payload.response.session_get_nvruim_data_sync.nvruim_3gpd_ctrl_data_enum =
        nvruim_data_ptr->data.nvruim_3gpd_ctrl_and_esn_usg.mmgsdi_nvruim_3gpd_control_data;

    rsp_ptr->rsp_payload.response.session_get_nvruim_data_sync.esn_usage_3gpd_enum =
        nvruim_data_ptr->data.nvruim_3gpd_ctrl_and_esn_usg.esn_usage_type;
    break;


    default:
        UIMDIAG_MSG_ERROR_0("Invalid NVRUIM Data Req Enum Type");
        return UIMDIAG_ERROR;

}
*/

  if(sizeof(nvruim_data_ptr->data) <= UIMDIAG_MAX_NVRUIM_DATA_SYNC)
  {
    memscpy(rsp_ptr->rsp_payload.response.session_get_nvruim_data_sync.nvruim_data_sync_rsp,
             sizeof(rsp_ptr->rsp_payload.response.session_get_nvruim_data_sync.nvruim_data_sync_rsp),
             &nvruim_data_ptr->data,
             (uint8)sizeof(nvruim_data_ptr->data));
  }
  return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_session_get_nvruim_data_sync_cnf */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_GENERIC_RSP_CB

DESCRIPTION
  Generic comand callback function, handles all mmgsdi command cnfs and
  prepares diag response packet

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void uimdiag_mmgsdi_generic_rsp_cb(
  mmgsdi_return_enum_type       mmgsdi_status,
  mmgsdi_cnf_enum_type          cnf_type,
  const mmgsdi_cnf_type       * cnf_data_ptr
)
{
  uimdiag_generic_rsp_type   *rsp_ptr                 = NULL;
  uint32                      len                     = 0;
  int32                       index                   = 0;
  boolean                     index_valid             = FALSE;
  uimdiag_return_enum_type    uimdiag_return          = UIMDIAG_ERROR;
  uint32                      temp_dynamic_len        = 0;
  mmgsdi_available_apps_type *available_apps_info_ptr = NULL;

  UIMDIAG_MSG_HIGH_2("uimdiag generic_rsp_cb: CNF Type is 0x%x & mmgsdi status is 0x%x",
                     cnf_type, mmgsdi_status);

  if (( cnf_data_ptr == NULL )||
      ( uimdiag_req_table_ptr == NULL ))
  {
    UIMDIAG_MSG_ERROR_0("Invalid cnf data or uimdiag_req_table_ptr is NULL");
    return;
  }

  /* calculate len because this cnf has dynamic response */
  switch(cnf_type)
  {
    case MMGSDI_CARD_STATUS_CNF:
      if( mmgsdi_status == MMGSDI_SUCCESS )
      {
        /*  Determine the length of the response pointer
            for header, Send Card Status Payload, 4 bytes for status */
        len = sizeof(uimdiag_cmd_rsp_hdr_type)+
              sizeof(uimdiag_mmgsdi_rsp_session_send_crd_status_pyld_type) + 4;
        /* Add size of Send Card Stauts Cnf data len -1
           one byte for data already available at
           uimdiag_mmgsdi_rsp_session_send_crd_status_pyld_type  */
        len = len + (cnf_data_ptr->status_cnf.status_data.data_len) - 1;
      }
      else
      {
        /* since command handling is not successful -
           allocate a default buffer to hold status */
        len = sizeof( uimdiag_generic_rsp_type );
      }
      break;
    case MMGSDI_READ_CNF:
      len = sizeof(uimdiag_cmd_rsp_hdr_type) +
          sizeof(uimdiag_mmgsdi_rsp_session_read_record_pyld_type) + 4;

      if(cnf_data_ptr->read_cnf.read_data.data_len > MMGSDI_MAX_DATA_BLOCK_LEN)
      {
        len += (cnf_data_ptr->read_cnf.read_data.data_len - MMGSDI_MAX_DATA_BLOCK_LEN);
      }
      break;
    case MMGSDI_SESSION_PERSO_CNF:
      len = sizeof(uimdiag_cmd_rsp_hdr_type) + 4;

      switch(cnf_data_ptr->session_perso_cnf.perso_action)
      {
        case MMGSDI_PERSO_ACTIVATE:
        case MMGSDI_PERSO_DEACTIVATE:
        case MMGSDI_PERSO_UNBLOCK:
          len += sizeof(uimdiag_mmgsdi_rsp_session_read_record_pyld_type);
          break;
        case MMGSDI_PERSO_GET_FEATURE_IND:
          len += sizeof(uimdiag_mmgsdi_rsp_session_perso_get_ind_pyld_type);
          break;
        case MMGSDI_PERSO_GET_DCK_RETRIES:
          len += sizeof(uimdiag_mmgsdi_rsp_session_perso_get_dck_retries_pyld_type);
          break;
        case MMGSDI_PERSO_LOCK_DOWN:
          len += sizeof(uimdiag_mmgsdi_rsp_session_perso_lock_dwn_enable_ltd_access_pyld_type);
          break;
        case MMGSDI_PERSO_GET_FEATURE_DATA:
          /* length is sum of get_data_pyld_type size and data_len - 1.
             one byte for data already available at get_data_pyld_type as data_ptr is
             a single byte array */
          len += sizeof(uimdiag_mmgsdi_rsp_session_perso_get_data_pyld_type) +
                (cnf_data_ptr->session_perso_cnf.perso_cnf_data.get_data_cnf.data_len-1);
          break;
        case MMGSDI_PERSO_SET_FEATURE_DATA:
          len += sizeof(uimdiag_mmgsdi_sessionid_type);
          break;
        case MMGSDI_PERSO_GET_FEATURE_KEY:
          len += sizeof(uimdiag_mmgsdi_rsp_session_perso_get_key_pyld_type);
          if(cnf_data_ptr->session_perso_cnf.perso_cnf_data.get_key_cnf.key.data_len >
               (UIMDIAG_MAX_PERSO_KEY_LENGTH + UIMDIAG_MAX_IMEI_LENGTH))
          {
            len += (cnf_data_ptr->session_perso_cnf.perso_cnf_data.get_key_cnf.key.data_len -
              (UIMDIAG_MAX_PERSO_KEY_LENGTH + UIMDIAG_MAX_IMEI_LENGTH));
          }
          break;
        default:
          len = sizeof(uimdiag_generic_rsp_type);
          break;
      }
      break;
    case MMGSDI_COMPUTE_IP_AUTH_CNF:
      len = sizeof(uimdiag_cmd_rsp_hdr_type) +
          sizeof(uimdiag_mmgsdi_session_compute_ip_auth_rsp_type) + 4;

      if(cnf_data_ptr->compute_ip_cnf.cmpt_ip_response_data.data_len > UIM_COMPUTE_IP_AUTH_RESPONSE_LENGTH)
      {
        len += (cnf_data_ptr->compute_ip_cnf.cmpt_ip_response_data.data_len - UIM_COMPUTE_IP_AUTH_RESPONSE_LENGTH);
      }
      break;
    case MMGSDI_SESSION_READ_PRL_CNF:
      len = sizeof(uimdiag_cmd_rsp_hdr_type) +
          sizeof(uimdiag_mmgsdi_session_read_prl_rsp_type) + 4;

      if(cnf_data_ptr->session_read_prl_cnf.size > UIMDIAG_MAX_ROAMING_LIST_LENGTH)
      {
        len += (cnf_data_ptr->session_read_prl_cnf.size - UIMDIAG_MAX_ROAMING_LIST_LENGTH);
      }
      break;
    case MMGSDI_SAP_CARD_READER_STATUS_CNF:
      len = sizeof(uimdiag_cmd_rsp_hdr_type) +
          sizeof(uimdiag_mmgsdi_sap_card_reader_status_rsp_type) + 4 +
      cnf_data_ptr->read_cnf.read_data.data_len;
      break;
    case MMGSDI_SESSION_OPEN_EXT_CNF:
      if(cnf_data_ptr->session_open_ext_cnf.num_sessions > UIMDIAG_MAX_SESSIONS)
      {
        UIMDIAG_MSG_ERROR_0("Number of session exceeds UIMDIAG_MAX_SESSIONS");
        return;
      }

      len = sizeof(uimdiag_cmd_rsp_hdr_type) +
          sizeof(uimdiag_mmgsdi_rsp_session_open_ext_pyld_type) + 4;

      if(cnf_data_ptr->session_open_ext_cnf.num_sessions > 1)
      {
        len = len + (sizeof(uimdiag_mmgsdi_session_open_info_type) *
              (cnf_data_ptr->session_open_ext_cnf.num_sessions - 1));
      }
      break;
    case MMGSDI_GET_ALL_AVAILABLE_APPS_CNF:
      if( mmgsdi_status == MMGSDI_SUCCESS )
      {
        /*  Determine the len of the response pointer  */

        /* for header, 4 bytes for status and one set app info by default */
        len = sizeof(uimdiag_cmd_rsp_hdr_type) +
              sizeof(uimdiag_mmgsdi_get_all_available_apps_rsp_type) + 4;

        /* len for one app is already included in
           uimdiag_mmgsdi_get_all_available_apps_rsp_type */
        if((cnf_data_ptr->get_all_available_apps_cnf.card1_num_app +
           cnf_data_ptr->get_all_available_apps_cnf.card2_num_app)  > 0)
        {
          len += ((cnf_data_ptr->get_all_available_apps_cnf.card1_num_app +
                  cnf_data_ptr->get_all_available_apps_cnf.card2_num_app) -1) *
                  sizeof(uimdiag_mmgsdi_app_info_type);
        }
      }
      else
      {
        /* since command handling is not successful -
           allocate a default buffer to hold status */
        len = sizeof( uimdiag_generic_rsp_type );
      }
      break;
    case MMGSDI_SESSION_GET_OPERATOR_NAME_CNF:
      if( mmgsdi_status == MMGSDI_SUCCESS )
      {
        uint32 plmn_index = 0;
        /***
             struct of response
             uint8    encoding_info;
             uint32   operator_name_len;
             uint8    operator_name[1];
             uint8    lang_ind;
             uint8    spn_display_bit;
             uint8    rplmn_display_bit;
             uint8    roaming_status;
             uint32   no_plmn_id;
             uint32   total_buff_size_of_operator_name_info;

             Dynamic part encoding
             uint8    plmn_id[3]
             uint8    rat_type
             uint8    long_name_ci;
             uint8    long_name_spare_bytes;
             uint8    long_name_encoding;
             uint32   long name len
             uint8    *long name
             uint8    short_name_ci;
             uint8    short_name_spare_bytes;
             uint8    short_name_encoding;
             uint32   short name len
             uint8    *short name
          */

        /* calculate len because this cnf has dynamic response */
        len = sizeof(uimdiag_cmd_rsp_hdr_type) + sizeof(uimdiag_mmgsdi_rsp_session_get_operator_name_type) + 4;

        /* operator name length - 1 byte as part of struct already */
        if (cnf_data_ptr->session_get_operator_name_cnf.spn.eons_data.data_len > 1)
        {
          temp_dynamic_len = (cnf_data_ptr->session_get_operator_name_cnf.spn.eons_data.data_len -1);
        }

        temp_dynamic_len += /* for plmn ids (3 bytes each) and RAT type (1 byte each) */
                            ((cnf_data_ptr->session_get_operator_name_cnf.plmn_list.num_of_plmn_ids) * 4) +
                            /* 1 each for encoding, ci and spare bytes for both long and short names */
                            ((cnf_data_ptr->session_get_operator_name_cnf.plmn_list.num_of_plmn_ids) * (6 * sizeof(uint8))) +
                            /* long name and short name length fields*/
                            ((cnf_data_ptr->session_get_operator_name_cnf.plmn_list.num_of_plmn_ids) * sizeof(uint32) * 2);

        for(plmn_index =0; plmn_index<cnf_data_ptr->session_get_operator_name_cnf.plmn_list.num_of_plmn_ids; plmn_index++)
        {
          temp_dynamic_len +=
            cnf_data_ptr->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[plmn_index].plmn_long_name.plmn_name.eons_data.data_len;
          temp_dynamic_len +=
            cnf_data_ptr->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[plmn_index].plmn_short_name.plmn_name.eons_data.data_len;
        }

        len += temp_dynamic_len;
      }
      else
      {
        /* since command handling is not successful -
           allocate a default buffer to hold status */
        len = sizeof( uimdiag_generic_rsp_type );
      }
      break;

    default:
      len = sizeof( uimdiag_generic_rsp_type );
      break;
  }

  index       = (int)cnf_data_ptr->response_header.client_data;
  index_valid = UIMDIAG_REQ_TABLE_IS_INDEX_VALID(index);

  if ( FALSE == index_valid )
  {
    UIMDIAG_MSG_ERROR_0("Invalid client data, index is invalid");
    return;
  }

  UIMDIAG_MSG_HIGH_1("Length to be allocated is %d", len);

  /* Allocate memory for the delayed response pointer */
  rsp_ptr =(uimdiag_generic_rsp_type  * )diagpkt_subsys_alloc_v2_delay(
            (diagpkt_subsys_id_type)DIAG_SUBSYS_UIM,
            (diagpkt_subsys_cmd_code_type)uimdiag_req_table_ptr[index].subcmd,
            (diagpkt_subsys_delayed_rsp_id_type)
            uimdiag_req_table_ptr[index].rsp_id,len );

  if(rsp_ptr == NULL)
  {
    /*do memory allocation for uimdiag_generic_rsp_type only*/
    len = sizeof( uimdiag_generic_rsp_type );

    rsp_ptr =(uimdiag_generic_rsp_type  * )diagpkt_subsys_alloc_v2_delay(
          (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
          (diagpkt_subsys_cmd_code_type)uimdiag_req_table_ptr[index].subcmd,
          (diagpkt_subsys_delayed_rsp_id_type)
          uimdiag_req_table_ptr[index].rsp_id,len );

    if(rsp_ptr == NULL)
    {
      uimdiag_free_index(index);
      return;
    }
    mmgsdi_status = MMGSDI_ERROR;

    diagpkt_subsys_set_status(rsp_ptr, (uint32) UIMDIAG_SUCCESS);
    rsp_ptr->rsp_payload.mmgsdi_status = mmgsdi_status;
    diagpkt_delay_commit(rsp_ptr);
    uimdiag_free_index(index);
    return;
  }

  switch(cnf_type)
  {
    case MMGSDI_CARD_STATUS_CNF:
      memset((void *)&rsp_ptr->rsp_payload,0x00,(len - sizeof(uimdiag_cmd_rsp_hdr_type)));
      break;
    case MMGSDI_SESSION_OPEN_EXT_CNF:
    case MMGSDI_READ_CNF:
    case MMGSDI_SESSION_PERSO_CNF:
    case MMGSDI_COMPUTE_IP_AUTH_CNF:
    case MMGSDI_SESSION_READ_PRL_CNF:
    case MMGSDI_SAP_CARD_READER_STATUS_CNF:
    case MMGSDI_SESSION_GET_OPERATOR_NAME_CNF:
    case MMGSDI_GET_ALL_AVAILABLE_APPS_CNF:
      memset(&rsp_ptr->rsp_payload,0x00,(len - sizeof(uimdiag_cmd_rsp_hdr_type)));
      break;
    default:
      memset(&rsp_ptr->rsp_payload,0x00, sizeof(uimdiag_cmd_rsp_pyld_type));
      break;
  }

  diagpkt_subsys_set_status(rsp_ptr, (uint32) UIMDIAG_SUCCESS);
  rsp_ptr->rsp_payload.mmgsdi_status = mmgsdi_status;

  if (MMGSDI_SUCCESS != mmgsdi_status )
  {
    /* extract cnf data only when cmd handling is successful */
    /* return all confirmation data (e.g. num of retires for PIN/ PERSO cases */
    switch(cnf_type)
    {
      case MMGSDI_PIN_OPERATION_CNF:
        break;

      case MMGSDI_SESSION_PERSO_CNF:
        switch(cnf_data_ptr->session_perso_cnf.perso_action)
        {
          case MMGSDI_PERSO_ACTIVATE:
          case MMGSDI_PERSO_DEACTIVATE:
          case MMGSDI_PERSO_UNBLOCK:
            break;
          default:
            diagpkt_delay_commit(rsp_ptr);
            uimdiag_free_index(index);
            return;
        } /* End of MMGSDI_SESSION_PERSO_CNF */
        break;

      default:
        diagpkt_delay_commit(rsp_ptr);
        uimdiag_free_index(index);
        return;
    }

    diagpkt_delay_commit(rsp_ptr);
    uimdiag_free_index(index);
    return;
  }

  switch(cnf_type)
  {
    case MMGSDI_CLIENT_ID_AND_EVT_REG_CNF:
      uimdiag_return =
        uimdiag_mmgsdi_proc_client_id_and_evt_reg_cnf(rsp_ptr,cnf_data_ptr);
      break;

    case MMGSDI_CLIENT_ID_AND_EVT_DEREG_CNF:
      uimdiag_return =
        uimdiag_mmgsdi_proc_client_id_and_evt_dereg_cnf(rsp_ptr,cnf_data_ptr);
      break;

    case MMGSDI_SESSION_OPEN_CNF:
      uimdiag_return =
        uimdiag_mmgsdi_proc_session_open_cnf(rsp_ptr,cnf_data_ptr);
      break;

    case MMGSDI_SESSION_CLOSE_CNF:
      uimdiag_return =
        uimdiag_mmgsdi_proc_session_close_cnf(rsp_ptr,cnf_data_ptr);
      break;

    case MMGSDI_SESSION_ACTIVATE_OR_SWITCH_PROVISIONING_CNF:
      uimdiag_return =
        uimdiag_mmgsdi_proc_act_or_switch_prov_cnf(rsp_ptr,cnf_data_ptr);
      break;

    case MMGSDI_SESSION_GET_INFO_CNF:
      uimdiag_return =
        uimdiag_mmgsdi_proc_get_info_cnf(rsp_ptr,cnf_data_ptr);
      break;

    case MMGSDI_GET_ALL_PIN_STATUS_CNF:
      uimdiag_return =
        uimdiag_mmgsdi_proc_get_all_pin_status_cnf(rsp_ptr,cnf_data_ptr);
      break;

    /* below PIN_OPERATION_CNF is called for verify_pin, disable pin
       enable_pin / change pin / unblock pin */
    case MMGSDI_PIN_OPERATION_CNF:
      uimdiag_return =
        uimdiag_mmgsdi_proc_pin_operation_cnf(rsp_ptr,cnf_data_ptr);
      break;

    case MMGSDI_GET_FILE_ATTR_CNF:
      uimdiag_return =
        uimdiag_mmgsdi_proc_get_file_attr_cnf(rsp_ptr,cnf_data_ptr);
      break;

  /* below READ_CNF is called for read_record/read_transparent */
    case MMGSDI_READ_CNF:
      uimdiag_return =
        uimdiag_mmgsdi_proc_read_record_cnf(rsp_ptr,cnf_data_ptr);
      break;

  /* below WRITR_CNF is called for write_record/write_transparent */
    case MMGSDI_WRITE_CNF:
      uimdiag_return =
        uimdiag_mmgsdi_proc_write_record_cnf(rsp_ptr,cnf_data_ptr);
      break;

    case MMGSDI_REFRESH_CNF:
      uimdiag_return =
        uimdiag_mmgsdi_refresh_cnf(rsp_ptr,cnf_data_ptr);
      break;

    case MMGSDI_SESSION_PERSO_CNF:
      uimdiag_return =
        uimdiag_mmgsdi_perso_cnf(rsp_ptr,cnf_data_ptr);
      break;

    case MMGSDI_SESSION_DEACTIVATE_CNF:
      uimdiag_return =
        uimdiag_mmgsdi_session_deactivate_prov_cnf(rsp_ptr,cnf_data_ptr);
      break;

    case MMGSDI_JCDMA_GET_CARD_INFO_CNF:
      uimdiag_return =
        uimdiag_mmgsdi_proc_session_jcdma_get_card_info_cnf(rsp_ptr,cnf_data_ptr);
      break;

    case MMGSDI_SESSION_GET_APP_CAPABILITIES_CNF:
      uimdiag_return =
        uimdiag_mmgsdi_session_get_app_cap_cnf(rsp_ptr,
                                      &(cnf_data_ptr->session_app_capabilities_cnf));
      break;

    case MMGSDI_SESSION_GET_CPHS_INFO_CNF:
      uimdiag_return =
        uimdiag_mmgsdi_session_get_cphs_info_cnf(rsp_ptr,cnf_data_ptr);
    break;

    case MMGSDI_ISIM_AUTH_CNF:
      uimdiag_return =
        uimdiag_mmgsdi_session_isim_auth_cnf(rsp_ptr,cnf_data_ptr);
    break;

    case MMGSDI_CARD_STATUS_CNF:
      uimdiag_return =
        uimdiag_mmgsdi_session_send_card_status_cnf(rsp_ptr, cnf_data_ptr, len);
    break;

    case MMGSDI_SESSION_DISABLE_SERVICE_CNF:
      uimdiag_return =
        uimdiag_mmgsdi_session_disable_service_cnf(rsp_ptr,cnf_data_ptr);
    break;

    case MMGSDI_SESSION_ENABLE_SERVICE_CNF:
      uimdiag_return =
        uimdiag_mmgsdi_session_enable_service_cnf(rsp_ptr,cnf_data_ptr);
    break;

    case MMGSDI_SRV_AVAILABLE_CNF:
      uimdiag_return =
        uimdiag_mmgsdi_srv_avail_cnf(rsp_ptr,cnf_data_ptr);
    break;

    case MMGSDI_SEARCH_CNF:
      uimdiag_return =
        uimdiag_mmgsdi_session_search_cnf(rsp_ptr,cnf_data_ptr);
        break;

    case MMGSDI_SESSION_PROV_APP_INIT_COMPLETE_CNF:
      uimdiag_return =
        uimdiag_mmgsdi_prov_app_init_cmpl_cnf(rsp_ptr,cnf_data_ptr);
    break;

    case MMGSDI_COMPUTE_IP_AUTH_CNF:
      uimdiag_return =
        uimdiag_mmgsdi_comp_ip_auth_cnf(rsp_ptr,cnf_data_ptr);
    break;

    case MMGSDI_SESSION_SSD_UPDATE_CNF:
      uimdiag_return =
        uimdiag_mmgsdi_ssd_update_cnf(rsp_ptr,cnf_data_ptr);
    break;

    case MMGSDI_SESSION_SSD_CONFIRM_CNF:
      uimdiag_return =
        uimdiag_mmgsdi_ssd_confirm_cnf(rsp_ptr,cnf_data_ptr);
    break;

    case MMGSDI_SESSION_BS_CHAL_CNF:
      uimdiag_return =
        uimdiag_mmgsdi_bs_chal_cnf(rsp_ptr,cnf_data_ptr);
    break;

    case MMGSDI_SESSION_READ_PRL_CNF:
      uimdiag_return =
        uimdiag_mmgsdi_read_prl_cnf(rsp_ptr,cnf_data_ptr);
    break;

    case MMGSDI_SAP_GET_ATR_CNF:
      uimdiag_return =
        uimdiag_mmgsdi_sap_get_atr_cnf(rsp_ptr,cnf_data_ptr);
      break;

    case MMGSDI_SAP_SEND_APDU_CNF:
      uimdiag_return =
        uimdiag_mmgsdi_sap_send_apdu_cnf(rsp_ptr,cnf_data_ptr);
      break;

    case MMGSDI_SAP_POWER_ON_CNF:
    case MMGSDI_SAP_POWER_OFF_CNF:
    case MMGSDI_SAP_RESET_CNF:
    case MMGSDI_SAP_CONNECT_CNF:
      uimdiag_return =
        uimdiag_mmgsdi_sap_generic_cnf(rsp_ptr,
          cnf_data_ptr,
          cnf_type);
      break;

    case MMGSDI_SAP_DISCONNECT_CNF:
      uimdiag_return = uimdiag_mmgsdi_sap_disconnect_cnf(rsp_ptr, cnf_data_ptr);
      break;

    case MMGSDI_SAP_CARD_READER_STATUS_CNF:
      uimdiag_return =
        uimdiag_mmgsdi_sap_card_reader_status_cnf(rsp_ptr,cnf_data_ptr);
      break;

    case MMGSDI_CARD_PDOWN_CNF:
      uimdiag_return =
        uimdiag_mmgsdi_card_pdown_cnf(rsp_ptr,cnf_data_ptr);
      break;

    case MMGSDI_CARD_PUP_CNF:
      uimdiag_return =
        uimdiag_mmgsdi_card_pup_cnf(rsp_ptr,cnf_data_ptr);
      break;

    case MMGSDI_SESSION_OPEN_EXT_CNF:
      uimdiag_return =
        uimdiag_mmgsdi_proc_session_open_ext_cnf(rsp_ptr, cnf_data_ptr, len);
      break;

    case MMGSDI_ACTIVATE_ONCHIP_SIM_CNF:
      uimdiag_return =
        uimdiag_mmgsdi_activate_onchip_sim_cnf(rsp_ptr,cnf_data_ptr);
      break;

    case MMGSDI_SEND_APDU_CNF:
      uimdiag_return =
        uimdiag_mmgsdi_send_apdu_cnf(rsp_ptr,cnf_data_ptr);
      break;

    case MMGSDI_GET_ALL_AVAILABLE_APPS_CNF:
      UIMDIAG_UTIL_TMC_MEM_MALLOC_AND_VALIDATE(
        available_apps_info_ptr,
        sizeof(mmgsdi_available_apps_type));

      if(available_apps_info_ptr == NULL)
      {
        uimdiag_return = UIMDIAG_MEMORY_ERROR_HEAP_EXHAUSTED;
        break;
      }

      available_apps_info_ptr->card1_num_app = cnf_data_ptr->get_all_available_apps_cnf.card1_num_app;
      available_apps_info_ptr->card2_num_app = cnf_data_ptr->get_all_available_apps_cnf.card2_num_app;
      memscpy(available_apps_info_ptr->card1_app_info, sizeof(available_apps_info_ptr->card1_app_info),
              cnf_data_ptr->get_all_available_apps_cnf.card1_app_info,
              sizeof(mmgsdi_app_info_type)*MMGSDI_MAX_APP_INFO);
      memscpy(available_apps_info_ptr->card2_app_info, sizeof(available_apps_info_ptr->card2_app_info),
              cnf_data_ptr->get_all_available_apps_cnf.card2_app_info,
              sizeof(mmgsdi_app_info_type)*MMGSDI_MAX_APP_INFO);

      uimdiag_return =
          uimdiag_mmgsdi_get_all_available_apps_cnf(rsp_ptr,available_apps_info_ptr);

      UIMDIAGUTIL_TMC_MEM_FREE(available_apps_info_ptr);
      break;

    case MMGSDI_SESSION_GET_OPERATOR_NAME_CNF:
      uimdiag_return =
        uimdiag_mmgsdi_get_operator_name_cnf(rsp_ptr,cnf_data_ptr);
      break;

    default:
      UIMDIAG_MSG_ERROR_1("Unexpected confiramtion type: %x ", cnf_type);
      uimdiag_return = UIMDIAG_ERROR;
      break;
  }

  if (uimdiag_return != UIMDIAG_SUCCESS )
  {
    UIMDIAG_MSG_ERROR_1("CNF: 0x%x handling failed", cnf_type);
    rsp_ptr->rsp_payload.mmgsdi_status = MMGSDI_ERROR;
  }

  uimdiag_free_index(index);
  diagpkt_delay_commit(rsp_ptr);
}/* uimdiag_mmgsdi_generic_rsp_cb */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_FREE_CMD_DATA

DESCRIPTION
  Function to free up allocated memory to command data

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void uimdiag_mmgsdi_free_cmd_data(
  uimdiag_generic_req_type              * req_ptr,
  uimdiag_subsystem_cmd_enum_type         subsys_cmd
)
{
  if (req_ptr == NULL )
  {
    UIMDIAG_MSG_ERROR_0("Invalid req ptr");
    return;
  }

  switch(subsys_cmd)
  {
  case UIMDIAG_MMGSDI_SESSION_OPEN_CMD:
    UIMDIAGUTIL_TMC_MEM_FREE(req_ptr->request.session_open.aid_data_ptr);
    break;

  case UIMDIAG_MMGSDI_SESSION_ACT_OR_SW_PROV_CMD:
    UIMDIAGUTIL_TMC_MEM_FREE(req_ptr->request.session_act_or_sw_prov.data_ptr);
    break;

  case UIMDIAG_MMGSDI_SESSION_VERIFY_PIN_CMD:
    UIMDIAGUTIL_TMC_MEM_FREE(req_ptr->request.session_verify_pin.pin_value_ptr);
    break;

  case UIMDIAG_MMGSDI_SESSION_DISABLE_PIN_CMD:
    UIMDIAGUTIL_TMC_MEM_FREE(req_ptr->request.session_disable_pin.pin_value_ptr);
    break;

  case UIMDIAG_MMGSDI_SESSION_ENABLE_PIN_CMD:
    UIMDIAGUTIL_TMC_MEM_FREE(req_ptr->request.session_enable_pin.pin_value_ptr);
    break;

  case UIMDIAG_MMGSDI_SESSION_CHANGE_PIN_CMD:
    UIMDIAGUTIL_TMC_MEM_FREE(req_ptr->request.session_change_pin.pin_value_ptr);
    UIMDIAGUTIL_TMC_MEM_FREE(req_ptr->request.session_change_pin.new_pin_value_ptr);
    break;

  case UIMDIAG_MMGSDI_SESSION_UNBLOCK_PIN_CMD:
    UIMDIAGUTIL_TMC_MEM_FREE(req_ptr->request.session_unblock_pin.puk_value_ptr);
    UIMDIAGUTIL_TMC_MEM_FREE(req_ptr->request.session_unblock_pin.new_pin_value_ptr);
    break;

  case UIMDIAG_MMGSDI_SESSION_GET_FILE_ATTR_CMD:
    UIMDIAGUTIL_TMC_MEM_FREE(req_ptr->request.session_get_file_attr.path_ptr);
    UIMDIAGUTIL_TMC_MEM_FREE(req_ptr->request.session_get_file_attr.aid_ptr);
    break;

  case UIMDIAG_MMGSDI_SESSION_READ_RECORD_CMD:
    UIMDIAGUTIL_TMC_MEM_FREE(req_ptr->request.session_read_record.path_ptr);
    UIMDIAGUTIL_TMC_MEM_FREE(req_ptr->request.session_read_record.aid_ptr);
    break;

  case UIMDIAG_MMGSDI_SESSION_READ_TRANSPARENT_CMD:
    UIMDIAGUTIL_TMC_MEM_FREE(req_ptr->request.session_read_transparent.path_ptr);
    UIMDIAGUTIL_TMC_MEM_FREE(req_ptr->request.session_read_transparent.aid_ptr);
    break;

  case UIMDIAG_MMGSDI_SESSION_WRITE_RECORD_CMD:
    UIMDIAGUTIL_TMC_MEM_FREE(req_ptr->request.session_write_record.path_ptr);
    UIMDIAGUTIL_TMC_MEM_FREE(req_ptr->request.session_write_record.aid_ptr);
    UIMDIAGUTIL_TMC_MEM_FREE(req_ptr->request.session_write_record.data_ptr);
    break;

  case UIMDIAG_MMGSDI_SESSION_WRITE_TRANSPARENT_CMD:
    UIMDIAGUTIL_TMC_MEM_FREE(req_ptr->request.session_write_transparent.path_ptr);
    UIMDIAGUTIL_TMC_MEM_FREE(req_ptr->request.session_write_transparent.aid_ptr);
    UIMDIAGUTIL_TMC_MEM_FREE(req_ptr->request.session_write_transparent.data_ptr);
    break;

  case UIMDIAG_MMGSDI_SESSION_REG_FOR_REFRESH_CMD:
  case UIMDIAG_MMGSDI_SESSION_DEREG_FOR_REFRESH_CMD:
  case UIMDIAG_MMGDDI_SESSION_REGISTER_FOR_REFRESH_PROCEED_VOTING_CMD:
    UIMDIAGUTIL_TMC_MEM_FREE(req_ptr->request.session_refresh.file_list_ptr);
    UIMDIAGUTIL_TMC_MEM_FREE(req_ptr->request.session_refresh.file_path_ptr);
    break;

  case UIMDIAG_MMGSDI_NAA_REFRESH_CMD:
    UIMDIAGUTIL_TMC_MEM_FREE(req_ptr->request.session_trigger_refresh.sim_app_id);
    UIMDIAGUTIL_TMC_MEM_FREE(req_ptr->request.session_trigger_refresh.path);
    break;

  case UIMDIAG_MMGSDI_SESSION_ISIM_AUTH_CMD:
    UIMDIAGUTIL_TMC_MEM_FREE(req_ptr->request.session_isim_auth.auth_data_ptr);
    break;

  case UIMDIAG_MMGSDI_SESSION_COMPUTE_IP_AUTH_CMD:
    UIMDIAGUTIL_TMC_MEM_FREE(req_ptr->request.session_compute_ip_auth.chap_challenge);
    UIMDIAGUTIL_TMC_MEM_FREE(req_ptr->request.session_compute_ip_auth.mn_ha_reg_data);
    UIMDIAGUTIL_TMC_MEM_FREE(req_ptr->request.session_compute_ip_auth.rrq_data);
    UIMDIAGUTIL_TMC_MEM_FREE(req_ptr->request.session_compute_ip_auth.mn_aaa_challenge);
    UIMDIAGUTIL_TMC_MEM_FREE(req_ptr->request.session_compute_ip_auth.hrpd_chap_challenge);
    break;

  case UIMDIAG_MMGSDI_SESSION_WRITE_CACHE_CMD:
    UIMDIAGUTIL_TMC_MEM_FREE(req_ptr->request.session_write_cache.data);
    break;

  case UIMDIAG_MMGSDI_ACTIVATE_ONCHIP_SIM_CMD:
  case UIMDIAG_MMGSDI_ACTIVATE_ONCHIP_USIM_CMD:
    UIMDIAGUTIL_TMC_MEM_FREE(req_ptr->request.activate_onchip_sim.onchip_data_ptr);
    break;

  case UIMDIAG_MMGSDI_SEND_APDU_CMD:
    UIMDIAGUTIL_TMC_MEM_FREE(req_ptr->request.send_apdu.apdu_data_ptr);
    break;

  default:
    break;
  }
} /* uimdiag_mmgsdi_free_cmd_data */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_PARSE_COMMANDS

DESCRIPTION
  Function to preparse and populate the response pointer with the conformation
  data

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type.

SIDE EFFECTS
  None.
===========================================================================*/
uimdiag_return_enum_type uimdiag_mmgsdi_parse_commands(
  PACKED void                           * cmd_ptr,
  uint16                                  pkt_len,
  uimdiag_generic_rsp_type             ** rsp_pptr,
  diagpkt_subsys_delayed_rsp_id_type    * rsp_id_ptr,
  int32                                 * index_ptr,
  uimdiag_generic_req_type              * request_ptr,
  uimdiag_subsystem_cmd_enum_type         subsys_cmd
)
{
  uimdiag_return_enum_type              uimdiag_status         = UIMDIAG_ERROR;

  UIMDIAG_UTIL_RETURN_IF_NULL_3(rsp_pptr, rsp_id_ptr, request_ptr);

  /*  allocate immediate response pointer  */
  *rsp_pptr = (uimdiag_generic_rsp_type *)
                diagpkt_subsys_alloc_v2(
                  (diagpkt_subsys_id_type) DIAG_SUBSYS_GSDI,
                  (diagpkt_subsys_cmd_code_type) subsys_cmd,
                  sizeof(uimdiag_generic_rsp_type));

  if (*rsp_pptr == NULL)
  {
    return UIMDIAG_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  memset(&(*rsp_pptr)->rsp_payload,0x00, sizeof(uimdiag_cmd_rsp_pyld_type));

  /* Initialize to MMGSDI_ERROR for the immediate response */
  (*rsp_pptr)->rsp_payload.mmgsdi_status = MMGSDI_ERROR;

  *rsp_id_ptr = diagpkt_subsys_get_delayed_rsp_id(*rsp_pptr );

  /* extract data from req packet
  and populate fields in the request structure */
  uimdiag_status = uimdiag_parse_cmd_data(request_ptr, cmd_ptr, pkt_len);

  if (uimdiag_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_1("uimdiag_mmgsdi_parse_commands: Parsing error 0x%x",
                        uimdiag_status);
    /* Do an immediate rsp - No delayed response expected*/
    diagpkt_subsys_reset_delayed_rsp_id(*rsp_pptr);
    diagpkt_subsys_set_status(*rsp_pptr, (uint32) UIMDIAG_SUCCESS);
    diagpkt_commit(*rsp_pptr);
    uimdiag_mmgsdi_free_cmd_data(request_ptr, subsys_cmd);
    return uimdiag_status;
  }
  /* find a free space in req_table to hold information for callback */
  *index_ptr = uimdiag_get_index();
  if (*index_ptr == UIMDIAG_GET_FN_ERR )
  {
    /*  req_table is full */
    uimdiag_status = UIMDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
    UIMDIAG_MSG_ERROR_1("uimdiag_mmgsdi_parse_commands: Parsing error 0x%x",
                        uimdiag_status);
    /* Do an immediate rsp - No delayed response expected*/
    diagpkt_subsys_reset_delayed_rsp_id(*rsp_pptr);
    diagpkt_subsys_set_status(*rsp_pptr, (uint32) UIMDIAG_SUCCESS);
    diagpkt_commit(*rsp_pptr);
    uimdiag_mmgsdi_free_cmd_data(request_ptr, subsys_cmd);
    return uimdiag_status;
  }
  return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_parse_commands */


/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_EVENT_CB

DESCRIPTION
    This function is the event handling function for different events from MMGSDI.

DEPENDENCIES
    None

INPUT
    mmgsdi_event_data_type *event_ptr

RETURN VALUE
    None

OUTPUT
    None
===========================================================================*/
static void uimdiag_mmgsdi_event_cb(const mmgsdi_event_data_type *event_ptr)
{
  uimdiag_mmgsdi_event_payload_type event_payload; /* used to send event*/
  uint8                             len    = 0;    /* length of the event_payload*/
  uint8                             offset = 0 ;

  if(event_ptr == NULL)
  {
    return;
  }

  UIMDIAG_MSG_HIGH_1("In uimdiag_mmgsdi_event_cb, Event is 0x%x",
                     event_ptr->evt);

  (void)memset((void*)&event_payload,0x00,sizeof(uimdiag_mmgsdi_event_payload_type));
  len   = sizeof(event_payload);

  event_payload.evt               = (uint8)event_ptr->evt;
  memscpy(event_payload.event_payload, sizeof(event_payload.event_payload),
          &event_ptr->session_id, UIMDIAG_SESSION_ID_SIZE);
  offset = offset + UIMDIAG_SESSION_ID_SIZE;

  switch (event_ptr->evt)
  {
    case MMGSDI_REFRESH_EVT:
      if( (offset+4) > EVENT_MAX_PAYLOAD)
      {
        return;
      }
      event_payload.event_payload[offset] = (uint8)event_ptr->data.refresh.slot;
      offset++;
      event_payload.event_payload[offset] = (uint8)event_ptr->data.refresh.stage;
      offset++;
      event_payload.event_payload[offset] = (uint8)event_ptr->data.refresh.mode;
      offset++;
      event_payload.event_payload[offset] = (uint8)event_ptr->data.refresh.orig_mode;
      break;

    case MMGSDI_FDN_EVT:
      if( (offset+3) > EVENT_MAX_PAYLOAD)
      {
        return;
      }
       /* Typecasting below to uint8 could be a problem, as the rec_num
         is 32 bits long*/
      event_payload.event_payload[offset] = (uint8)event_ptr->data.fdn.slot;
      offset++;
      event_payload.event_payload[offset] = (uint8)event_ptr->data.fdn.enabled;
      offset++;
      event_payload.event_payload[offset] = (uint8)event_ptr->data.fdn.rec_num;
      break;

    case MMGSDI_SUBSCRIPTION_READY_EVT:
      if( (offset+3) > EVENT_MAX_PAYLOAD)
      {
        return;
      }
      event_payload.event_payload[offset] = (uint8)event_ptr->data.subscription_ready.app_info.slot;
      offset++;
      event_payload.event_payload[offset] = (uint8)event_ptr->data.subscription_ready.app_info.app_data.app_type;
      offset++;
      event_payload.event_payload[offset] = (uint8)event_ptr->data.subscription_ready.app_info.cached;
      break;

    case MMGSDI_PERSO_EVT:
      if( (offset+4) > EVENT_MAX_PAYLOAD)
      {
        return;
      }
      event_payload.event_payload[offset] = (uint8)event_ptr->data.perso.feature;
      offset++;
      event_payload.event_payload[offset] = (uint8)event_ptr->data.perso.status;
      offset++;
      event_payload.event_payload[offset] = (uint8)event_ptr->data.perso.num_retries;
      offset++;
      event_payload.event_payload[offset] = (uint8)event_ptr->data.perso.num_unblock_retries;
      offset++;
      break;

    case MMGSDI_SESSION_ILLEGAL_SUBSCRIPTION_EVT:
      if( (offset+1) > EVENT_MAX_PAYLOAD)
      {
        return;
      }
      event_payload.event_payload[offset] = (uint8)event_ptr->data.illegal_session.session_id;
      break;

    case MMGSDI_SESSION_CHANGED_EVT:
      if( (offset+3) > EVENT_MAX_PAYLOAD)
      {
        return;
      }
      event_payload.event_payload[offset] = (uint8)event_ptr->data.session_changed.activated;
      offset++;
      event_payload.event_payload[offset] = (uint8)event_ptr->data.session_changed.app_info.cached;
      offset++;
      event_payload.event_payload[offset] = (uint8)event_ptr->data.session_changed.app_info.slot;
      break;

    case MMGSDI_CARD_INSERTED_EVT:
      if( (offset+2) > EVENT_MAX_PAYLOAD)
      {
        return;
      }
      event_payload.event_payload[offset] = (uint8)event_ptr->data.card_inserted.slot;
      offset++;
      event_payload.event_payload[offset] = (uint8)event_ptr->data.card_inserted.num_aids_avail;
      break;

    case MMGSDI_CARD_ERROR_EVT:
      if( (offset+2) > EVENT_MAX_PAYLOAD)
      {
        return;
      }
      event_payload.event_payload[offset] = (uint8)event_ptr->data.card_error.slot;
      offset++;
      event_payload.event_payload[offset] = (uint8)event_ptr->data.card_error.info;
      break;

    case MMGSDI_CARD_REMOVED_EVT:
      if( (offset+1) > EVENT_MAX_PAYLOAD)
      {
        return;
      }
      event_payload.event_payload[offset] = (uint8)event_ptr->data.card_removed.slot;
      break;

    case MMGSDI_TERMINAL_PROFILE_DL_EVT:
      if( (offset+3) > EVENT_MAX_PAYLOAD)
      {
        return;
      }
      event_payload.event_payload[offset] = (uint8)event_ptr->data.terminal_profile.slot;
      offset++;
      event_payload.event_payload[offset] = (uint8)event_ptr->data.terminal_profile.protocol;
      offset++;
      event_payload.event_payload[offset] = (uint8)event_ptr->data.terminal_profile.ok_dl;
      break;

    case MMGSDI_PIN1_EVT:
    case MMGSDI_PIN2_EVT:
    case MMGSDI_UNIVERSAL_PIN_EVT:
      if( (offset+4) > EVENT_MAX_PAYLOAD)
      {
        return;
      }
      event_payload.event_payload[offset] = (uint8)event_ptr->data.pin.slot;
      offset++;
      event_payload.event_payload[offset] = (uint8)event_ptr->data.pin.pin_info.pin_id;
      offset++;
      event_payload.event_payload[offset] = (uint8)event_ptr->data.pin.pin_info.status;
      offset++;
      event_payload.event_payload[offset] = (uint8)event_ptr->data.pin.pin_info.num_retries;
      break;

    case MMGSDI_EONS_OPL_PNN_CACHE_READY_EVT:
      if( (offset+6) > EVENT_MAX_PAYLOAD)
      {
        return;
      }
      event_payload.event_payload[offset] = (uint8)event_ptr->data.eons_opl_pnn_cache_ready.opl_info.num_of_rec;
      offset++;
      event_payload.event_payload[offset] = (uint8)event_ptr->data.eons_opl_pnn_cache_ready.opl_info.rec_len;
      offset++;
      event_payload.event_payload[offset] = (uint8)event_ptr->data.eons_opl_pnn_cache_ready.opl_info.valid_record_list.data_len;
      offset++;
      event_payload.event_payload[offset] = (uint8)event_ptr->data.eons_opl_pnn_cache_ready.pnn_info.num_of_rec;
      offset++;
      event_payload.event_payload[offset] = (uint8)event_ptr->data.eons_opl_pnn_cache_ready.pnn_info.rec_len;
      offset++;
      event_payload.event_payload[offset] = (uint8)event_ptr->data.eons_opl_pnn_cache_ready.pnn_info.valid_record_list.data_len;
      break;

    default:
      /* For unhandled MMGSDI events return without sending diag event */
      return;
  }/*switch (event_ptr->evt)*/

  event_report_payload( EVENT_MMGSDI_EVENT, len, (void *)&event_payload);
}/* uimdiag_mmgsdi_event_cb */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_POST_PARSE_COMMANDS

DESCRIPTION
  Function post parsing the command to commit delayed response

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
uimdiag_return_enum_type uimdiag_mmgsdi_post_parse_commands(
  uimdiag_generic_rsp_type             ** rsp_pptr,
  diagpkt_subsys_delayed_rsp_id_type      rsp_id,
  uimdiag_return_enum_type                uimdiag_status,
  int32                                   index,
  mmgsdi_return_enum_type                 mmgsdi_status,
  uimdiag_generic_req_type              * request_ptr,
  uimdiag_subsystem_cmd_enum_type         subsys_cmd
)
{
  uimdiag_generic_rsp_type               * delayed_rsp_ptr = NULL;
  if (rsp_pptr == NULL)
  {
    UIMDIAG_MSG_ERROR_0("rsp_pptr == NULL");
    return UIMDIAG_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  if (request_ptr == NULL)
  {
    UIMDIAG_MSG_ERROR_0("request_ptr == NULL");
    return UIMDIAG_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  diagpkt_subsys_set_status(* rsp_pptr, (uint32) UIMDIAG_SUCCESS);
  diagpkt_commit(* rsp_pptr);

  if ( MMGSDI_SUCCESS != mmgsdi_status )
  {
    /* error in handling API , we need a delayed response too */
    UIMDIAG_MSG_ERROR_2("FAILED TO QUEUE MMGSDI SESSION COMMAND, status: 0x%x for cmd: 0x%x",
                        mmgsdi_status, subsys_cmd);
    uimdiag_free_index(index);
    delayed_rsp_ptr = (uimdiag_generic_rsp_type *)
                        diagpkt_subsys_alloc_v2_delay(
                          (diagpkt_subsys_id_type) DIAG_SUBSYS_GSDI,
                          (diagpkt_subsys_cmd_code_type) subsys_cmd,
                           rsp_id,
                           sizeof(uimdiag_generic_rsp_type));

    if (delayed_rsp_ptr == NULL)
    {
      uimdiag_mmgsdi_free_cmd_data(request_ptr, subsys_cmd);
      return UIMDIAG_ERROR;
    }
    memset(&delayed_rsp_ptr->rsp_payload,0x00, sizeof(uimdiag_cmd_rsp_pyld_type));
    delayed_rsp_ptr->rsp_payload.mmgsdi_status = mmgsdi_status;
    diagpkt_subsys_set_status(delayed_rsp_ptr,(uint32)UIMDIAG_SUCCESS);
    rex_sleep(10);
    diagpkt_delay_commit(delayed_rsp_ptr);
  }
  /*Set the immediate response to SUCCESS*/
  (*rsp_pptr)->rsp_payload.mmgsdi_status = MMGSDI_SUCCESS;
  return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_post_parse_commands */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_OPEN_CMD

DESCRIPTION
  Command handler for session open subsystem command

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_open_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type              request;
  uimdiag_generic_rsp_type             *rsp_ptr                = NULL;
  uimdiag_return_enum_type              delayed_rsp_status     = UIMDIAG_ERROR;
  diagpkt_subsys_delayed_rsp_id_type    rsp_id                 = 0;
  uimdiag_return_enum_type              uimdiag_status         = UIMDIAG_ERROR;
  uimdiag_mmgsdi_session_open_cmd_type *session_open_req_ptr   = NULL;
  mmgsdi_client_id_type                 client_id              = 0;
  int32                                 index                  = 0;
  mmgsdi_return_enum_type               mmgsdi_status          = MMGSDI_ERROR;
  mmgsdi_non_prov_app_info_type         non_prov_app_data;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_open_cmd");

  (void)memset ( (void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );
  (void)memset ( (void*)&non_prov_app_data, 0x00,
                 sizeof(mmgsdi_non_prov_app_info_type));

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,pkt_len,&rsp_ptr,&rsp_id,&index,
                     &request,UIMDIAG_MMGSDI_SESSION_OPEN_CMD);

  if((uimdiag_status != UIMDIAG_SUCCESS))
  {
    return rsp_ptr;
  }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd = UIMDIAG_MMGSDI_SESSION_OPEN_CMD;

  session_open_req_ptr = &request.request.session_open;
  UIMDIAG_TOUINT64(client_id,session_open_req_ptr->client_id_low,
                   session_open_req_ptr->client_id_high);

  if((session_open_req_ptr->session_type ==
     (uint32) MMGSDI_NON_PROV_SESSION_SLOT_1) ||
     (session_open_req_ptr->session_type ==
     (uint32) MMGSDI_NON_PROV_SESSION_SLOT_2) ||
     (session_open_req_ptr->session_type ==
     (uint32) MMGSDI_NON_PROV_SESSION_SLOT_3))
  {
     non_prov_app_data.exclusive_channel    =
     (boolean)session_open_req_ptr->exclusive_channel;

     non_prov_app_data.app_id_data.data_len =
     uint32toint32(session_open_req_ptr->aid_data_len);

     if (session_open_req_ptr->aid_data_ptr != NULL)
      {
       non_prov_app_data.app_id_data.data_ptr = session_open_req_ptr->aid_data_ptr;
      }
     else
     {
       UIMDIAG_MSG_ERROR_0("UIMDIAG Session Open: Non Prov APP AID Data Pointer is NULL");
     }

   mmgsdi_status = mmgsdi_session_open (
                    client_id,
                    (mmgsdi_session_type_enum_type)
                    session_open_req_ptr->session_type,
                    &non_prov_app_data,
                    uimdiag_mmgsdi_event_cb,
                    (boolean)session_open_req_ptr->set_notify,
                    (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                    (mmgsdi_client_data_type)index);
  }
  else
  {

   mmgsdi_status = mmgsdi_session_open (
                    client_id,
                    (mmgsdi_session_type_enum_type)
                    session_open_req_ptr->session_type,
                    NULL,
                    uimdiag_mmgsdi_event_cb,
                    (boolean)session_open_req_ptr->set_notify,
                    (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                    (mmgsdi_client_data_type)index);
  }

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SESSION_OPEN_CMD);

  if( delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG Session Open: Delayed Response error");
    return NULL;
  }

  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_open.aid_data_ptr);
  return rsp_ptr;
}/* uimdiag_mmgsdi_session_open_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_CLOSE_CMD

DESCRIPTION
  Command handler for session close subsystem command

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_close_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  int32                              index              = 0;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_session_id_type             session_id;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_close_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

 uimdiag_status = uimdiag_mmgsdi_parse_commands(
                    cmd_ptr,
                    pkt_len,
                    &rsp_ptr,
                    &rsp_id,
                    &index,
                    &request,
                    UIMDIAG_MMGSDI_SESSION_CLOSE_CMD);

  if(uimdiag_status != UIMDIAG_SUCCESS)
  {
    return rsp_ptr;
  }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd = UIMDIAG_MMGSDI_SESSION_CLOSE_CMD;
  session_id = request.request.session_close.session_id_low;

  mmgsdi_status = mmgsdi_session_close( session_id,
                    (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                    (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SESSION_CLOSE_CMD
    );

  if(delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG Session Close:Delayed Response error");
    return NULL;
  }
  return rsp_ptr;
}/* uimdiag_mmgsdi_session_close_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_CLIENT_ID_AND_EVT_REG_CMD

DESCRIPTION
  Command handler for client id and evt reg subsystem command

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_client_id_and_evt_reg_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  int32                              index              = 0;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_client_id_and_evt_reg_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_CLIENT_ID_AND_EVT_REG_CMD);

  if(uimdiag_status != UIMDIAG_SUCCESS)
  {
    return rsp_ptr;
  }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                        UIMDIAG_MMGSDI_CLIENT_ID_AND_EVT_REG_CMD;

  mmgsdi_status =   mmgsdi_client_id_and_evt_reg(uimdiag_mmgsdi_event_cb,
                      (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                      (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_CLIENT_ID_AND_EVT_REG_CMD
    );

  if(delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG Client ID and Evt Reg: Delayed Response error");
    return NULL;
  }

  return rsp_ptr;
}/* uimdiag_mmgsdi_client_id_and_evt_reg_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_CLIENT_ID_AND_EVT_DEREG_CMD

DESCRIPTION
  Command handler for client id and evt dereg subsystem command

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_client_id_and_evt_dereg_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type             request;
  uimdiag_generic_rsp_type            *rsp_ptr            = NULL;
  diagpkt_subsys_delayed_rsp_id_type   rsp_id             = 0;
  uimdiag_return_enum_type             uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type             delayed_rsp_status = UIMDIAG_ERROR;
  int32                                index           = 0;
  mmgsdi_client_id_type                client_id       = 0;
  mmgsdi_return_enum_type              mmgsdi_status   = MMGSDI_ERROR;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_client_id_and_evt_dereg_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_CLIENT_ID_AND_EVT_DEREG_CMD);

  if(uimdiag_status != UIMDIAG_SUCCESS)
  {
    return rsp_ptr;
  }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                        UIMDIAG_MMGSDI_CLIENT_ID_AND_EVT_DEREG_CMD;

  UIMDIAG_TOUINT64(client_id,request.request.client_id_and_evt_dereg.client_id_low,
                   request.request.client_id_and_evt_dereg.client_id_high);

  mmgsdi_status =  mmgsdi_client_id_and_evt_dereg(client_id,
                     (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                     (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_CLIENT_ID_AND_EVT_DEREG_CMD);

  if(delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG Client ID and Evt Dereg: Delayed Response error");
    return NULL;
  }

  return rsp_ptr;
}/* uimdiag_mmgsdi_client_id_and_evt_dereg_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_ACT_OR_SW_PROV_CMD

DESCRIPTION
  Command handler for session activate or Switch Provisioning

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_act_or_sw_prov_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  int32                              index              = 0;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  mmgsdi_session_id_type             session_id;
  mmgsdi_slot_id_enum_type           card_slot;
  mmgsdi_data_type                   app_id_data;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_act_or_switch_prov_cmd");
  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

 uimdiag_status = uimdiag_mmgsdi_parse_commands(
                    cmd_ptr,
                    pkt_len,
                    &rsp_ptr,
                    &rsp_id,
                    &index,
                    &request,
                    UIMDIAG_MMGSDI_SESSION_ACT_OR_SW_PROV_CMD);

  if(uimdiag_status != UIMDIAG_SUCCESS)
  {
    return rsp_ptr;
  }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                    UIMDIAG_MMGSDI_SESSION_ACT_OR_SW_PROV_CMD;

  session_id = request.request.session_act_or_sw_prov.session_id_low;
  card_slot  = (mmgsdi_slot_id_enum_type)
                 request.request.session_act_or_sw_prov.slot_id;

  app_id_data.data_len = request.request.session_act_or_sw_prov.data_len;
  app_id_data.data_ptr = request.request.session_act_or_sw_prov.data_ptr;

  mmgsdi_status =  mmgsdi_session_activate_or_switch_provisioning (
                     session_id,
                     card_slot,
                     app_id_data,
                     (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                     (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SESSION_ACT_OR_SW_PROV_CMD);

  if(delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG Session Act or Switch: Delayed Response error");
    return NULL;
  }

  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_act_or_sw_prov.data_ptr);
  return rsp_ptr;
}/* uimdiag_mmgsdi_session_act_or_sw_prov_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_GET_INFO_CMD

DESCRIPTION
  Command handler for session get info subsystem command

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_get_info_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type          request;
  uimdiag_generic_rsp_type          *rsp_ptr             = NULL;
  uimdiag_return_enum_type          uimdiag_status       = UIMDIAG_ERROR;
  uimdiag_return_enum_type          delayed_rsp_status   = UIMDIAG_ERROR;
  int32                             index                = 0;
  mmgsdi_return_enum_type           mmgsdi_status        = MMGSDI_ERROR;
  diagpkt_subsys_delayed_rsp_id_type rsp_id              = 0;
  mmgsdi_session_id_type            session_id;
  mmgsdi_session_info_query_type    session_info_query;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_get_info_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );
  (void)memset((void*)&session_info_query, 0x00, sizeof(mmgsdi_session_info_query_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_GET_INFO_CMD);

   if(uimdiag_status != UIMDIAG_SUCCESS)
   {
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd = UIMDIAG_MMGSDI_SESSION_GET_INFO_CMD;
  session_id = request.request.session_get_info.session_id_low;

  session_info_query.query_type = (mmgsdi_session_info_enum_type)
    request.request.session_get_info.session_info;

  if(session_info_query.query_type == MMGSDI_SESSION_INFO_QUERY_BY_ID)
  {
    session_info_query.query_key.session_id = session_id;
  }
  else if(session_info_query.query_type == MMGSDI_SESSION_INFO_QUERY_BY_TYPE)
  {
    session_info_query.query_key.session_type = (mmgsdi_session_type_enum_type)
      request.request.session_get_info.session_type;
  }

  mmgsdi_status =   mmgsdi_session_get_info(
                      session_info_query ,
                      (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                      (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SESSION_GET_INFO_CMD);

  if(delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG Session Get info: Delayed Response error");
    return NULL;
  }

  return rsp_ptr;
}/* uimdiag_mmgsdi_session_get_info_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_GET_ALL_PIN_STATUS_CMD

DESCRIPTION
  Command handler to request the PIN status for all relevant PINs applicable
  to the session ID provided.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_get_all_pin_status_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr             = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id              = 0;
  uimdiag_return_enum_type           uimdiag_status      = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status  = UIMDIAG_ERROR;
  int32                              index               = 0;
  mmgsdi_return_enum_type            mmgsdi_status       = MMGSDI_ERROR;
  mmgsdi_session_id_type             session_id;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_get_all_pin_status_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_GET_ALL_PIN_STATUS_CMD);

   if(uimdiag_status != UIMDIAG_SUCCESS)
   {
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd = UIMDIAG_MMGSDI_SESSION_GET_ALL_PIN_STATUS_CMD;

  session_id = request.request.session_get_all_pin_stat.session_id_low;

  mmgsdi_status = mmgsdi_session_get_all_pin_status ( session_id,
                      (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                      (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SESSION_GET_ALL_PIN_STATUS_CMD);

  if(delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG Session_get_all_pin_status:Delayed Response error");
    return NULL;
  }

  return rsp_ptr;
}/* uimdiag_mmgsdi_session_get_all_pin_status_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_VERIFY_PIN_CMD

DESCRIPTION
  Command handler to perform a PIN verification procedure against a card for
  PIN1, PIN2 or UPIN.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_verify_pin_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type                    request;
  uimdiag_generic_rsp_type                   *rsp_ptr                = NULL;
  uimdiag_mmgsdi_session_verify_pin_cmd_type *session_verify_pin_ptr = NULL;
  diagpkt_subsys_delayed_rsp_id_type          rsp_id                 = 0;
  int32                                       index                  = 0;
  mmgsdi_return_enum_type                     mmgsdi_status          =
                                                                  MMGSDI_ERROR;
  uimdiag_return_enum_type                    uimdiag_status         =
                                                                  UIMDIAG_ERROR;
  uimdiag_return_enum_type                    delayed_rsp_status     =
                                                                  UIMDIAG_ERROR;
  mmgsdi_session_id_type                      session_id;
  mmgsdi_data_type                            pin_data;
  mmgsdi_pin_enum_type                        pin_id;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_verify_pin_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_VERIFY_PIN_CMD);

   if(uimdiag_status != UIMDIAG_SUCCESS)
   {
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd = UIMDIAG_MMGSDI_SESSION_VERIFY_PIN_CMD;

  session_verify_pin_ptr = &request.request.session_verify_pin;

  session_id             = session_verify_pin_ptr->session_id_low;
  pin_id                 = (mmgsdi_pin_enum_type)
                              session_verify_pin_ptr->pin_id;
  pin_data.data_len      = uint32toint32(session_verify_pin_ptr->pin_len);


  if (session_verify_pin_ptr->pin_value_ptr != NULL)
  {
    pin_data.data_ptr    = session_verify_pin_ptr->pin_value_ptr;
  }
  else
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG Session Verify Pin: Pin Value Pointer is NULL");
    return NULL;
  }

  mmgsdi_status = mmgsdi_session_verify_pin(
                    session_id, pin_id, pin_data,
                    (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                    (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SESSION_VERIFY_PIN_CMD);

  if(delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG session_verify_pin:Delayed Response error");
    return NULL;
  }

  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_verify_pin.pin_value_ptr);
  return rsp_ptr;
}/* uimdiag_mmgsdi_session_verify_pin_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_DISABLE_PIN_CMD

DESCRIPTION
  Command handler to build a request to MMGSDI to disable the PIN provided

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_disable_pin_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  int32                              index              = 0;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_session_id_type             session_id;
  mmgsdi_pin_enum_type               pin_id;
  mmgsdi_pin_replace_enum_type       replace_pin;
  mmgsdi_data_type                   pin_data;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_disable_pin_cmd");
  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_DISABLE_PIN_CMD);

   if(uimdiag_status != UIMDIAG_SUCCESS)
   {
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd = UIMDIAG_MMGSDI_SESSION_DISABLE_PIN_CMD;

  session_id         = request.request.session_disable_pin.session_id_low;
  pin_id             = (mmgsdi_pin_enum_type)
                          request.request.session_disable_pin.pin_id;
  replace_pin        = (mmgsdi_pin_replace_enum_type)
                          request.request.session_disable_pin.pin_replace;
  pin_data.data_len  = request.request.session_disable_pin.pin_len;
  pin_data.data_ptr  = request.request.session_disable_pin.pin_value_ptr;

  mmgsdi_status =   mmgsdi_session_disable_pin (
                      session_id,
                      pin_id,
                      replace_pin,
                      pin_data,
                      (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                      (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SESSION_DISABLE_PIN_CMD);

  if(delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG session_disable:Delayed Response error");
    return NULL;
  }

  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_disable_pin.pin_value_ptr);
  return rsp_ptr;
}/* uimdiag_mmgsdi_session_disable_pin_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_ENABLE_PIN_CMD

DESCRIPTION
  Command handler to build a request to MMGSDI to enable the PIN provided

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_enable_pin_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  int32                              index              = 0;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_pin_enum_type               pin_id;
  mmgsdi_data_type                   pin_data;
  mmgsdi_session_id_type             session_id;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_enable_pin_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_ENABLE_PIN_CMD);

   if(uimdiag_status != UIMDIAG_SUCCESS)
   {
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd = UIMDIAG_MMGSDI_SESSION_ENABLE_PIN_CMD;

  session_id        = request.request.session_enable_pin.session_id_low;
  pin_id            = (mmgsdi_pin_enum_type)
                        request.request.session_enable_pin.pin_id;
  pin_data.data_len = request.request.session_enable_pin.pin_len;
  pin_data.data_ptr = request.request.session_enable_pin.pin_value_ptr;

  mmgsdi_status =   mmgsdi_session_enable_pin (
                      session_id,
                      pin_id,
                      pin_data,
                      (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                      (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SESSION_ENABLE_PIN_CMD
    );

  if(delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG session_enable_pin:Delayed Response error");
    return NULL;
  }

  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_enable_pin.pin_value_ptr);
  return rsp_ptr;
}/* uimdiag_mmgsdi_session_enable_pin_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_CHANGE_PIN_CMD

DESCRIPTION
  Command handler to build a request to MMGSDI to change the PIN provided

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_change_pin_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  int32                              index              = 0;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_pin_enum_type               pin_id;
  mmgsdi_data_type                   pin_data;
  mmgsdi_data_type                   new_pin_data;
  mmgsdi_session_id_type             session_id;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_change_pin_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_CHANGE_PIN_CMD);

   if(uimdiag_status != UIMDIAG_SUCCESS)
   {
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd = UIMDIAG_MMGSDI_SESSION_CHANGE_PIN_CMD;

  session_id            = request.request.session_change_pin.session_id_low;
  pin_id                = (mmgsdi_pin_enum_type)
                            request.request.session_change_pin.pin_id;
  pin_data.data_len     = request.request.session_change_pin.pin_len;
  pin_data.data_ptr     = request.request.session_change_pin.pin_value_ptr;
  new_pin_data.data_len = request.request.session_change_pin.new_pin_len;
  new_pin_data.data_ptr = request.request.session_change_pin.new_pin_value_ptr;

  mmgsdi_status =   mmgsdi_session_change_pin (
                      session_id,
                      pin_id,
                      pin_data,
                      new_pin_data,
                      (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                      (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SESSION_CHANGE_PIN_CMD
    );

  if(delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG session_change_pin:Delayed Response error");
    return NULL;
  }

  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_change_pin.pin_value_ptr);
  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_change_pin.new_pin_value_ptr);
  return rsp_ptr;
}/* uimdiag_mmgsdi_session_change_pin_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_UNBLOCK_PIN_CMD

DESCRIPTION
  Command handler to build a request to MMGSDI to unblock the PIN provided

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_unblock_pin_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  int32                              index              = 0;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_pin_enum_type               pin_id;
  mmgsdi_data_type                   puk_data;
  mmgsdi_data_type                   new_pin_data;
  mmgsdi_session_id_type             session_id;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_unblock_pin_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_UNBLOCK_PIN_CMD);

   if(uimdiag_status != UIMDIAG_SUCCESS)
   {
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd = UIMDIAG_MMGSDI_SESSION_UNBLOCK_PIN_CMD;

  session_id            = request.request.session_unblock_pin.session_id_low;
  pin_id                = (mmgsdi_pin_enum_type)
                            request.request.session_unblock_pin.pin_id;
  puk_data.data_len     = request.request.session_unblock_pin.puk_len;
  puk_data.data_ptr     = request.request.session_unblock_pin.puk_value_ptr;
  new_pin_data.data_len = request.request.session_unblock_pin.new_pin_len;
  new_pin_data.data_ptr = request.request.session_unblock_pin.new_pin_value_ptr;

  mmgsdi_status =   mmgsdi_session_unblock_pin (
                      session_id,
                      pin_id,
                      puk_data,
                      new_pin_data,
                      (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                      (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SESSION_UNBLOCK_PIN_CMD
    );

  if(delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG session_unblock_pin:Delayed Response error");
    return NULL;
  }

  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_unblock_pin.puk_value_ptr);
  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_unblock_pin.new_pin_value_ptr);
  return rsp_ptr;
}/* uimdiag_mmgsdi_session_unblock_pin_cmd */

/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_SESSION_GET_FILE_ATTR_CMD

DESCRIPTION
  Command handler to build a request to MMGSDI to get file attr

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_get_file_attr_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr             = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id              = 0;
  uimdiag_return_enum_type           uimdiag_status      = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status  = UIMDIAG_ERROR;
  int32                              index               = 0;
  mmgsdi_return_enum_type            mmgsdi_status       = MMGSDI_ERROR;

  mmgsdi_session_id_type             session_id;
  mmgsdi_access_type                  file_name;


  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_get_file_attr_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );
  (void)memset((void*)&file_name, 0x00, sizeof(mmgsdi_access_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_GET_FILE_ATTR_CMD);

  if(uimdiag_status != UIMDIAG_SUCCESS)
    {
     return rsp_ptr;
    }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd = UIMDIAG_MMGSDI_SESSION_GET_FILE_ATTR_CMD;

  session_id                  = request.request.session_get_file_attr.session_id_low;

  file_name.access_method     = (mmgsdi_access_method_enum_type) request.request.session_get_file_attr.access_method;

  switch(file_name.access_method)
  {
   case MMGSDI_EF_ENUM_ACCESS:
        file_name.file.file_enum = uimdiag_util_convert_to_mmgsdi_items_enum
                                     (request.request.session_get_file_attr.ef_enum);
        break;

   case MMGSDI_DF_ENUM_ACCESS:
        file_name.file.df_enum = (mmgsdi_df_enum_type)
                                   request.request.session_get_file_attr.df_enum;
        break;

   case MMGSDI_BY_PATH_ACCESS:
        file_name.file.path_type.path_len = request.request.session_get_file_attr.path_len;
        if ( (request.request.session_get_file_attr.path_len > 0) &&
             (request.request.session_get_file_attr.path_len  <=  MMGSDI_MAX_PATH_LEN))
          {
           memscpy(file_name.file.path_type.path_buf,
           sizeof(file_name.file.path_type.path_buf),
           request.request.session_get_file_attr.path_ptr,
           (request.request.session_get_file_attr.path_len*sizeof(uint16)));
          }
        else
        {
          UIMDIAG_MSG_ERROR_0("path_len could be <= 0 or exceeds max path len size ");
          return NULL;
          }
        break;

   case MMGSDI_BY_APP_ID_ACCESS:
        file_name.file.app_id.data_len = request.request.session_get_file_attr.aid_len;
        if (request.request.session_get_file_attr.aid_len != 0 )
          {
           memscpy(file_name.file.app_id.data_ptr,
           sizeof(file_name.file.app_id.data_ptr),
           request.request.session_get_file_attr.aid_ptr,
           request.request.session_get_file_attr.aid_len);
          }
        break;

   default:
        break;
  }

  mmgsdi_status =   mmgsdi_session_get_file_attr (
                    session_id,
                    file_name,
                    (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                    (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SESSION_GET_FILE_ATTR_CMD
    );

  if(delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG session_get_file_attr:Delayed Response error");
    return NULL;
  }

  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_get_file_attr.path_ptr);
  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_get_file_attr.aid_ptr);
  return rsp_ptr;
}/* uimdiag_mmgsdi_session_get_file_attr_cmd */

/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_SESSION_READ_RECORD_CMD

DESCRIPTION
  Command handler to build a request to MMGSDI to read record

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_read_record_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr             = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id              = 0;
  uimdiag_return_enum_type           uimdiag_status      = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status  = UIMDIAG_ERROR;
  int32                              index               = 0;
  mmgsdi_return_enum_type            mmgsdi_status       = MMGSDI_ERROR;

  mmgsdi_session_id_type             session_id;
  mmgsdi_access_type                 file_access;
  mmgsdi_rec_num_type                record_number;
  mmgsdi_len_type                    request_length;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_read_record_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );
  (void)memset((void*)&file_access, 0x00, sizeof(mmgsdi_access_type));

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_READ_RECORD_CMD);

   if(uimdiag_status != UIMDIAG_SUCCESS)
   {
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd = UIMDIAG_MMGSDI_SESSION_READ_RECORD_CMD;

  session_id                    = request.request.session_read_record.session_id_low;
  record_number           = request.request.session_read_record.rec_num;
  request_length                = request.request.session_read_record.rec_len;

  file_access.access_method     = (mmgsdi_access_method_enum_type) request.request.session_read_record.access_method;

  switch(file_access.access_method)
  {
  case MMGSDI_EF_ENUM_ACCESS:
     file_access.file.file_enum = uimdiag_util_convert_to_mmgsdi_items_enum(
                                    request.request.session_read_record.ef_enum);
     break;

  case MMGSDI_DF_ENUM_ACCESS:
     file_access.file.df_enum = (mmgsdi_df_enum_type)
                                 request.request.session_read_record.df_enum;
     break;

  case MMGSDI_BY_PATH_ACCESS:
     file_access.file.path_type.path_len = request.request.session_read_record.path_len;
     if( (request.request.session_read_record.path_len > 0 ) &&
         (request.request.session_read_record.path_len <= MMGSDI_MAX_PATH_LEN))
     {
       memscpy(file_access.file.path_type.path_buf,
         sizeof(file_access.file.path_type.path_buf),
         request.request.session_read_record.path_ptr,
         (request.request.session_read_record.path_len*sizeof(uint16)));
     }
     else
     {
       UIMDIAG_MSG_ERROR_0("path_len could be <= 0 or exceeds max path len size ");
       return NULL;
     }
     break;

  case MMGSDI_BY_APP_ID_ACCESS:
     file_access.file.app_id.data_len = request.request.session_read_record.aid_len;
     if (request.request.session_read_record.aid_len != 0 )
     {
      memscpy(file_access.file.app_id.data_ptr,
      sizeof(file_access.file.app_id.data_ptr),
      request.request.session_read_record.aid_ptr,
      request.request.session_read_record.aid_len);
     }
     break;

  default:
     break;

  }

  mmgsdi_status =   mmgsdi_session_read_record (
                      session_id,
                      file_access,
            record_number,
            request_length,
                      (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                      (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SESSION_READ_RECORD_CMD
    );

  if(delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG session_read_record :Delayed Response error");
    return NULL;
  }

  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_read_record.path_ptr);
  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_read_record.aid_ptr);
  return rsp_ptr;
}/* uimdiag_mmgsdi_session_read_record_cmd */

/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_SESSION_READ_TRANSPARENT_CMD

DESCRIPTION
  Command handler to build a request to MMGSDI to read transparent file

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_read_transparent_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr             = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id              = 0;
  uimdiag_return_enum_type           uimdiag_status      = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status  = UIMDIAG_ERROR;
  int32                              index               = 0;
  mmgsdi_return_enum_type            mmgsdi_status       = MMGSDI_ERROR;

  mmgsdi_session_id_type             session_id;
  mmgsdi_access_type                 file_access;
  mmgsdi_offset_type         offset;
  mmgsdi_len_type          req_len;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_read_transparent_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );
  (void)memset((void*)&file_access, 0x00, sizeof(mmgsdi_access_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_READ_TRANSPARENT_CMD);

   if(uimdiag_status != UIMDIAG_SUCCESS)
   {
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd = UIMDIAG_MMGSDI_SESSION_READ_TRANSPARENT_CMD;

  session_id                = request.request.session_read_transparent.session_id_low;
  offset        = request.request.session_read_transparent.offset;
  req_len       = request.request.session_read_transparent.req_len;

  file_access.access_method = (mmgsdi_access_method_enum_type)
                              request.request.session_read_transparent.access_method;

  switch(file_access.access_method)
  {
  case MMGSDI_EF_ENUM_ACCESS:
       file_access.file.file_enum = uimdiag_util_convert_to_mmgsdi_items_enum(
                                      request.request.session_read_transparent.ef_enum);
       break;

  case MMGSDI_DF_ENUM_ACCESS:
       file_access.file.df_enum = (mmgsdi_df_enum_type)
                                  request.request.session_read_transparent.df_enum;
       break;

  case MMGSDI_BY_PATH_ACCESS:
       file_access.file.path_type.path_len = request.request.session_read_transparent.path_len;
       if ( (request.request.session_read_transparent.path_len > 0 ) &&
            (request.request.session_read_transparent.path_len <= MMGSDI_MAX_PATH_LEN) )
         {
          memscpy(file_access.file.path_type.path_buf,
          sizeof(file_access.file.path_type.path_buf),
          request.request.session_read_transparent.path_ptr,
         (request.request.session_read_transparent.path_len*sizeof(uint16)));
       }
       else
       {
         UIMDIAG_MSG_ERROR_0("path_len could be <= 0 or exceeds max path len size ");
         return NULL;
         }
       break;

  case MMGSDI_BY_APP_ID_ACCESS:
       file_access.file.app_id.data_len = request.request.session_read_transparent.aid_len;
       if (request.request.session_read_transparent.aid_len != 0 )
         {
          memscpy(file_access.file.app_id.data_ptr,
          sizeof(file_access.file.app_id.data_ptr),
          request.request.session_read_transparent.aid_ptr,
          request.request.session_read_transparent.aid_len);
         }
       break;

  default:
       break;

  }

  mmgsdi_status =   mmgsdi_session_read_transparent (
                      session_id,
                      file_access,
          offset,
          req_len,
                      (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                      (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SESSION_READ_TRANSPARENT_CMD
    );

  if(delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG session_read_transparent:Delayed Response error");
    return NULL;
  }

  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_read_transparent.path_ptr);
  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_read_transparent.aid_ptr);
  return rsp_ptr;
}/* uimdiag_mmgsdi_session_read_transparent_cmd */

/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_SESSION_WRITE_RECORD_CMD

DESCRIPTION
  Command handler to build a request to MMGSDI to write record

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_write_record_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr             = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id              = 0;
  uimdiag_return_enum_type           uimdiag_status      = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status  = UIMDIAG_ERROR;
  int32                              index               = 0;
  mmgsdi_return_enum_type            mmgsdi_status       = MMGSDI_ERROR;

  mmgsdi_session_id_type             session_id;
  mmgsdi_access_type                 file_access;
  mmgsdi_file_structure_enum_type    record_type;
  mmgsdi_rec_num_type                record_number;
  mmgsdi_write_data_type             write_data;



  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_write_record_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );
  (void)memset((void*)&file_access, 0x00, sizeof(mmgsdi_access_type) );
  (void)memset((void*)&write_data, 0x00, sizeof(mmgsdi_write_data_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_WRITE_RECORD_CMD);

   if(uimdiag_status != UIMDIAG_SUCCESS)
   {
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd = UIMDIAG_MMGSDI_SESSION_WRITE_RECORD_CMD;

  session_id          = (mmgsdi_session_id_type)
                          request.request.session_write_record.session_id_low;
  record_type       = (mmgsdi_file_structure_enum_type)
                          request.request.session_write_record.rec_type;
  record_number       = request.request.session_write_record.rec_num;
  write_data.data_len = request.request.session_write_record.data_len;

  if(request.request.session_write_record.data_len != 0)
  {
    write_data.data_ptr = request.request.session_write_record.data_ptr;
  }
  else
  {
    UIMDIAG_MSG_ERROR_0("request.request.session_write_record.data_len == 0");
  }

  file_access.access_method     = (mmgsdi_access_method_enum_type) request.request.session_write_record.access_method;

  switch(file_access.access_method)
  {
  case MMGSDI_EF_ENUM_ACCESS:
       file_access.file.file_enum = uimdiag_util_convert_to_mmgsdi_items_enum(
                                      request.request.session_write_record.ef_enum);
       break;

  case MMGSDI_DF_ENUM_ACCESS:
       file_access.file.df_enum = (mmgsdi_df_enum_type)
                                  request.request.session_write_record.df_enum;
       break;

  case MMGSDI_BY_PATH_ACCESS:
       file_access.file.path_type.path_len = request.request.session_write_record.path_len;
       if ( (request.request.session_write_record.path_len > 0) &&
            (request.request.session_write_record.path_len <= MMGSDI_MAX_PATH_LEN))
         {
          memscpy(file_access.file.path_type.path_buf,
          sizeof(file_access.file.path_type.path_buf),
          request.request.session_write_record.path_ptr,
          (request.request.session_write_record.path_len*sizeof(uint16)));
        }
       else
       {
         UIMDIAG_MSG_ERROR_0("path_len could be <= 0 or exceeds max path len size ");
         return NULL;
        }
       break;

  case MMGSDI_BY_APP_ID_ACCESS:
       file_access.file.app_id.data_len = request.request.session_write_record.aid_len;
       if (request.request.session_write_record.aid_len != 0 )
         {
          memscpy(file_access.file.app_id.data_ptr,
          sizeof(file_access.file.app_id.data_ptr),
          request.request.session_write_record.aid_ptr,
          request.request.session_write_record.aid_len);
         }
       break;

  default:
       break;
  }

  mmgsdi_status =   mmgsdi_session_write_record (
                      session_id,
                      file_access,
          record_type,
          record_number,
          write_data,
                      (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                      (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SESSION_WRITE_RECORD_CMD
    );

  if(delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG session_write_record :Delayed Response error");
    return NULL;
  }

  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_write_record.path_ptr);
  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_write_record.aid_ptr);
  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_write_record.data_ptr);
  return rsp_ptr;
}/* uimdiag_mmgsdi_session_write_record_cmd */

/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_SESSION_WRITE_TRANSPARENT_CMD

DESCRIPTION
  Command handler to build a request to MMGSDI to write to transparent file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_write_transparent_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr             = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id              = 0;
  uimdiag_return_enum_type           uimdiag_status      = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status  = UIMDIAG_ERROR;
  int32                              index               = 0;
  mmgsdi_return_enum_type            mmgsdi_status       = MMGSDI_ERROR;

  mmgsdi_session_id_type             session_id;
  mmgsdi_access_type                 file_access;
  mmgsdi_offset_type         offset;
  mmgsdi_write_data_type             write_data;



  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_write_transparent_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );
  (void)memset((void*)&file_access, 0x00, sizeof(mmgsdi_access_type) );
  (void)memset((void*)&write_data, 0x00, sizeof(mmgsdi_write_data_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_WRITE_TRANSPARENT_CMD);

   if(uimdiag_status != UIMDIAG_SUCCESS)
   {
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd = UIMDIAG_MMGSDI_SESSION_WRITE_TRANSPARENT_CMD;

  session_id                          = request.request.session_write_transparent.session_id_low;
  offset                = request.request.session_write_transparent.offset;
  write_data.data_len                 = request.request.session_write_transparent.data_len;

  if(request.request.session_write_transparent.data_len != 0)
  {
    write_data.data_ptr             = request.request.session_write_transparent.data_ptr;
  }
  else
  {
    UIMDIAG_MSG_ERROR_0("request.request.session_write_transparent.data_len == 0");
  }

  file_access.access_method = (mmgsdi_access_method_enum_type)
                              request.request.session_write_transparent.access_method;

  switch(file_access.access_method)
  {
  case MMGSDI_EF_ENUM_ACCESS:
       file_access.file.file_enum = uimdiag_util_convert_to_mmgsdi_items_enum(
                                      request.request.session_write_transparent.ef_enum);
       break;

  case MMGSDI_DF_ENUM_ACCESS:
       file_access.file.df_enum = (mmgsdi_df_enum_type)
                                  request.request.session_write_transparent.df_enum;
       break;

  case MMGSDI_BY_PATH_ACCESS:
       file_access.file.path_type.path_len = request.request.session_write_transparent.path_len;
       if ( (request.request.session_write_transparent.path_len > 0) &&
            (request.request.session_write_transparent.path_len <= MMGSDI_MAX_PATH_LEN) )
       {
         memscpy(file_access.file.path_type.path_buf,
           sizeof(file_access.file.path_type.path_buf),
           request.request.session_write_transparent.path_ptr,
           (request.request.session_write_transparent.path_len*sizeof(uint16)));
       }
       else
       {
         UIMDIAG_MSG_ERROR_0("path_len could be <= 0 or exceeds max path len size ");
         return NULL;
       }
       break;

  case MMGSDI_BY_APP_ID_ACCESS:
       file_access.file.app_id.data_len = request.request.session_write_transparent.aid_len;
       if (request.request.session_write_transparent.aid_len != 0 )
       {
         memscpy(file_access.file.app_id.data_ptr,
         sizeof(file_access.file.app_id.data_ptr),
         request.request.session_write_transparent.aid_ptr,
         request.request.session_write_transparent.aid_len);
       }
       break;

  default:
       break;
  }

  mmgsdi_status =   mmgsdi_session_write_transparent (
                      session_id,
                      file_access,
            offset,
            write_data,
            (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                      (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SESSION_WRITE_TRANSPARENT_CMD
    );


  if(delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG session_write_transparent:Delayed Response error");
    return NULL;
  }

  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_write_transparent.path_ptr);
  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_write_transparent.aid_ptr);
  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_write_transparent.data_ptr);
  return rsp_ptr;
}/* uimdiag_mmgsdi_session_write_transparent_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_REG_FOR_REFRESH_CMD

DESCRIPTION
  Command handler to build a request to register for Refresh to MMGSDI

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_reg_for_refresh_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr             = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id              = 0;
  int32                              index               = 0;
  uint32                             file_index          = 0;
  uimdiag_return_enum_type           uimdiag_status      = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status  = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status       = MMGSDI_ERROR;
  mmgsdi_session_id_type             session_id;
  mmgsdi_refresh_file_list_type      refresh_files;
  boolean                            vote_for_init;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_reg_for_refresh_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  (void)memset ((void*)&refresh_files, 0x00,
                 sizeof(mmgsdi_refresh_file_list_type));

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_REG_FOR_REFRESH_CMD);

   if(uimdiag_status != UIMDIAG_SUCCESS)
   {
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                                    UIMDIAG_MMGSDI_SESSION_REG_FOR_REFRESH_CMD;

  session_id                  =
                        request.request.session_refresh.session_id_low;
  refresh_files.num_files     =
                         request.request.session_refresh.num_of_files;

  UIMDIAG_UTIL_TMC_MEM_MALLOC_AND_VALIDATE(
    refresh_files.file_list_ptr,
    (request.request.session_refresh.num_of_files * sizeof(mmgsdi_file_enum_type)));
  if(refresh_files.file_list_ptr == NULL)
  {
    return NULL;
  }

  if(request.request.session_refresh.num_of_files > 0)
  {
    for (file_index = 0; file_index < request.request.session_refresh.num_of_files; file_index++)
    {
      refresh_files.file_list_ptr[file_index] =
                                       uimdiag_util_convert_to_mmgsdi_items_enum(
                                         request.request.session_refresh.file_list_ptr[file_index]);
    }
  }
  else
  {
    refresh_files.file_list_ptr= NULL;
  }

  refresh_files.file_path_len =
                         request.request.session_refresh.file_path_len;
  refresh_files.file_path_ptr =
                         request.request.session_refresh.file_path_ptr;
  vote_for_init               = (boolean)
                         request.request.session_refresh.vote_for_init;

  mmgsdi_status               =   mmgsdi_session_register_for_refresh  (
                                    session_id,
                                    refresh_files,
                                    vote_for_init,
                                    (mmgsdi_callback_type)
                                      uimdiag_mmgsdi_generic_rsp_cb,
                                    (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SESSION_REG_FOR_REFRESH_CMD
    );

  if(delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG session_reg_for_refresh:Delayed Response error");
    return NULL;
  }

  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_refresh.file_list_ptr);
  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_refresh.file_path_ptr);

  return rsp_ptr;
}/* uimdiag_mmgsdi_session_reg_for_refresh_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_REG_FOR_REFRESH_PROCEEED_VOTING_CMD

DESCRIPTION
  Command handler to build a request to register for Refresh to MMGSDI aling
  with proceed voting

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_reg_for_refresh_proceed_voting_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr             = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id              = 0;
  int32                              index               = 0;
  uimdiag_return_enum_type           uimdiag_status      = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status  = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status       = MMGSDI_ERROR;
  mmgsdi_session_id_type             session_id;
  mmgsdi_refresh_file_list_type      refresh_files;
  boolean                            vote_for_init;
  boolean                            proceed_with_refresh;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_reg_for_refresh_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  (void)memset ((void*)&refresh_files, 0x00,
                 sizeof(mmgsdi_refresh_file_list_type));

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGDDI_SESSION_REGISTER_FOR_REFRESH_PROCEED_VOTING_CMD);

   if(uimdiag_status != UIMDIAG_SUCCESS)
   {
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                                    UIMDIAG_MMGDDI_SESSION_REGISTER_FOR_REFRESH_PROCEED_VOTING_CMD;

  session_id                  =
                        request.request.session_refresh_proceed_voting.session_id_low;
  refresh_files.num_files     =
                         request.request.session_refresh_proceed_voting.num_of_files;
  refresh_files.file_list_ptr = (mmgsdi_file_enum_type *)
                         request.request.session_refresh_proceed_voting.file_list_ptr;
  refresh_files.file_path_len =
                         request.request.session_refresh_proceed_voting.file_path_len;
  refresh_files.file_path_ptr =
                         request.request.session_refresh_proceed_voting.file_path_ptr;
  vote_for_init               = (boolean)
                         request.request.session_refresh_proceed_voting.vote_for_init;
  proceed_with_refresh        = (boolean)
                         request.request.session_refresh_proceed_voting.proceed_refresh;


 mmgsdi_status = mmgsdi_session_register_for_refresh_ok_to_proceed_voting(
                    session_id,
                    refresh_files,
                    vote_for_init,
                    proceed_with_refresh,
                    (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                    (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGDDI_SESSION_REGISTER_FOR_REFRESH_PROCEED_VOTING_CMD
    );

  if(delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG session_reg_for_refresh:Delayed Response error");
    return NULL;
  }

  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_refresh.file_list_ptr);
  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_refresh.file_path_ptr);

  return rsp_ptr;
}/* uimdiag_mmgsdi_session_reg_for_refresh_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_NAA_REFRESH_CMD

DESCRIPTION
  Command handler to build a request to trigger for Refresh to MMGSDI

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_naa_refresh_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type             request;
  uimdiag_generic_rsp_type            *rsp_ptr             = NULL;
  diagpkt_subsys_delayed_rsp_id_type   rsp_id              = 0;
  int32                                index               = 0;
  uimdiag_return_enum_type             uimdiag_status      = UIMDIAG_ERROR;
  uimdiag_return_enum_type             delayed_rsp_status  = UIMDIAG_ERROR;
  mmgsdi_return_enum_type              mmgsdi_status       = MMGSDI_ERROR;
  mmgsdi_client_id_type                client_id           = 0;
  uimdiag_mmgsdi_naa_refresh_cmd_type *naa_refresh_ptr     = NULL;
  mmgsdi_option_type                   option;
  mmgsdi_slot_id_enum_type             card_slot;
  mmgsdi_aid_type                      aid;
  mmgsdi_refresh_mode_enum_type        refresh_mode;
  uint8                                num_files;
  mmgsdi_data_type                     refresh_files;


  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_naa_refresh_cmd");

  (void)memset((void *)&option, 0x00, sizeof(mmgsdi_option_type));
  (void)memset((void *)&request, 0x00, sizeof(uimdiag_generic_req_type));
  (void)memset((void *)&refresh_files, 0x00, sizeof(mmgsdi_data_type));
  (void)memset((void *)&aid, 0x00, sizeof(mmgsdi_aid_type));

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_NAA_REFRESH_CMD);

   if(uimdiag_status != UIMDIAG_SUCCESS)
   {
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                                    UIMDIAG_MMGSDI_NAA_REFRESH_CMD;

  naa_refresh_ptr = &request.request.session_trigger_refresh;

  UIMDIAG_TOUINT64(client_id,naa_refresh_ptr->client_id_low,
                   naa_refresh_ptr->client_id_high);


  card_slot              = (mmgsdi_slot_id_enum_type)
                            request.request.session_trigger_refresh.slot_id;

  aid.app_type           = MMGSDI_APP_NONE;

  aid.aid.data_len       = (mmgsdi_len_type)
                            request.request.session_trigger_refresh.sim_app_id_len;

  if(aid.aid.data_len > 0)
  {
    memscpy(aid.aid.data_ptr,
           sizeof(aid.aid.data_ptr),
           request.request.session_trigger_refresh.sim_app_id,
           aid.aid.data_len*sizeof(uint8));
  }
  else
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG: uimdiag_mmgsdi_naa_refresh aid length <= 0  ");
  }


  aid.label.data_len     = 0;
  memset(aid.label.data_ptr,0, MMGSDI_MAX_AID_LEN);


  refresh_mode           = (mmgsdi_refresh_mode_enum_type)
                            request.request.session_trigger_refresh.refresh_mode;

  num_files              = 0; /* num_files set to zero, as it is no longer used.*/

  refresh_files.data_len = (mmgsdi_len_type)
                            request.request.session_trigger_refresh.path_len;
  refresh_files.data_ptr = request.request.session_trigger_refresh.path;

  mmgsdi_status = mmgsdi_naa_refresh(
                    client_id,
                    card_slot,
                    aid,
                    refresh_mode,
                    refresh_files,
                    num_files,
                    NULL,
                    option,
                    (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                    (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_NAA_REFRESH_CMD
    );

  if(delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG uimdiag_mmgsdi_naa_refresh_cmd:Delayed Response error");
    return NULL;
  }

  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_trigger_refresh.sim_app_id);
  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_trigger_refresh.path);

  return rsp_ptr;
}/* uimdiag_mmgsdi_naa_refresh_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_OK_TO_REFRESH_CMD

DESCRIPTION
  Command handler to build a request to simulate OK to Refresh to MMGSDI

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_ok_to_refresh_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr             = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id              = 0;
  int32                              index               = 0;
  uimdiag_return_enum_type           uimdiag_status      = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status  = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status       = MMGSDI_ERROR;
  mmgsdi_session_id_type             session_id;
  boolean                            ok_to_refresh;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_ok_to_refresh_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_OK_TO_REFRESH_CMD);

   if(uimdiag_status != UIMDIAG_SUCCESS)
   {
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                                    UIMDIAG_MMGSDI_SESSION_OK_TO_REFRESH_CMD;

  session_id                  =
                        request.request.session_ok_to_refresh.session_id_low;
  ok_to_refresh               = (boolean)
                         request.request.session_ok_to_refresh.ok_to_refresh;

  mmgsdi_status               =   mmgsdi_session_ok_to_refresh  (
                                    session_id,
                                    ok_to_refresh,
                                    (mmgsdi_callback_type)
                                      uimdiag_mmgsdi_generic_rsp_cb,
                                    (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SESSION_OK_TO_REFRESH_CMD
    );

  if(delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG session_ok_to_refresh:Delayed Response error");
    return NULL;
  }

  return rsp_ptr;
}/* uimdiag_mmgsdi_session_ok_to_refresh_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_OK_TO_PROCEED_WITH_REFRESH_CMD

DESCRIPTION
  Command handler to build a request to simulate proceed with Refresh to MMGSDI

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_ok_to_proceed_with_refresh_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr             = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id              = 0;
  int32                              index               = 0;
  uimdiag_return_enum_type           uimdiag_status      = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status  = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status       = MMGSDI_ERROR;
  mmgsdi_session_id_type             session_id;
  boolean                            ok_to_proceed;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_ok_to_refresh_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_OK_TO_PROCEED_WITH_REFRESH_CMD);

   if(uimdiag_status != UIMDIAG_SUCCESS)
   {
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd = UIMDIAG_MMGSDI_SESSION_OK_TO_PROCEED_WITH_REFRESH_CMD;

  session_id    = request.request.session_refresh_ok_to_proceed.session_id_low;
  ok_to_proceed = (boolean)request.request.session_refresh_ok_to_proceed.ok_to_proceed;

  mmgsdi_status = mmgsdi_session_ok_to_proceed_with_refresh(
                    session_id,
                    ok_to_proceed,
                    (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                    (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SESSION_OK_TO_PROCEED_WITH_REFRESH_CMD
    );

  if(delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG session_ok_to_refresh:Delayed Response error");
    return NULL;
  }

  return rsp_ptr;
}/* uimdiag_mmgsdi_session_ok_to_refresh_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_REFRESH_COMPLETE_CMD

DESCRIPTION
  Command handler to build a request to indicate to MMGSDI that the client
  has finished its refresh process

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_refresh_complete_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  int32                              index              = 0;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_session_id_type             session_id;
  boolean                            pass_fail;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_refresh_complete_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_REFRESH_COMPLETE_CMD);

   if(uimdiag_status != UIMDIAG_SUCCESS)
   {
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                                    UIMDIAG_MMGSDI_SESSION_REFRESH_COMPLETE_CMD;

  session_id                  =
                        request.request.session_refresh_complete.session_id_low;
  pass_fail                   = (boolean)
                         request.request.session_refresh_complete.pass_fail;

  mmgsdi_status               =   mmgsdi_session_refresh_complete   (
                                    session_id,
                                    pass_fail,
                                    (mmgsdi_callback_type)
                                      uimdiag_mmgsdi_generic_rsp_cb,
                                    (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SESSION_REFRESH_COMPLETE_CMD
    );

  if(delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG session_refresh_complete:Delayed Response error");
    return NULL;
  }

  return rsp_ptr;
}/* uimdiag_mmgsdi_session_refresh_complete_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_DEREG_FOR_REFRESH_CMD

DESCRIPTION
  Command handler to build a request to deregister files that were registered
  for Refresh to MMGSDI

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_dereg_for_refresh_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  int32                              index              = 0;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_session_id_type             session_id;
  mmgsdi_refresh_file_list_type      refresh_files;
  boolean                            vote_for_init;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_dereg_for_refresh_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );
  (void)memset((void*)&refresh_files, 0x00, sizeof(mmgsdi_refresh_file_list_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_DEREG_FOR_REFRESH_CMD);

   if(uimdiag_status != UIMDIAG_SUCCESS)
   {
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                                    UIMDIAG_MMGSDI_SESSION_DEREG_FOR_REFRESH_CMD;

  session_id                  =
                        request.request.session_refresh.session_id_low;
  refresh_files.num_files     =
                         request.request.session_refresh.num_of_files;
  refresh_files.file_list_ptr = (mmgsdi_file_enum_type *)
                         request.request.session_refresh.file_list_ptr;
  refresh_files.file_path_len =
                         request.request.session_refresh.file_path_len;
  refresh_files.file_path_ptr =
                         request.request.session_refresh.file_path_ptr;
  vote_for_init               = (boolean)
                         request.request.session_refresh.vote_for_init;

  mmgsdi_status               =   mmgsdi_session_deregister_for_refresh  (
                                    session_id,
                                    refresh_files,
                                    vote_for_init,
                                    (mmgsdi_callback_type)
                                      uimdiag_mmgsdi_generic_rsp_cb,
                                    (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
                         &rsp_ptr,
                         rsp_id,
                         uimdiag_status,
                         index,
                         mmgsdi_status,
                         &request,
                         UIMDIAG_MMGSDI_SESSION_DEREG_FOR_REFRESH_CMD);

  if(delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG session_dereg_for_refresh:Delayed Response error");
    return NULL;
  }

  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_refresh.file_list_ptr);
  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_refresh.file_path_ptr);

  return rsp_ptr;
}/* uimdiag_mmgsdi_session_dereg_for_refresh_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_PERSO_ACTIVATE_CMD

DESCRIPTION
  Command handler to build a request to activate a personalization feature
  by an external source

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_perso_activate_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  int32                              index              = 0;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_perso_client_data_type      key_info;
  mmgsdi_session_id_type             session_id;
  mmgsdi_perso_feature_enum_type     feature;


  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_perso_activate_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );
  (void)memset ( (void*)&key_info, 0x00,
                 sizeof(mmgsdi_perso_client_data_type));

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_PERSO_ACTIVATE_CMD);

   if(uimdiag_status != UIMDIAG_SUCCESS)
   {
     UIMDIAG_MSG_HIGH_0("Session Perso Activate: Error in uimdiag_mmgsdi_parse_command");
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                                    UIMDIAG_MMGSDI_SESSION_PERSO_ACTIVATE_CMD;

  session_id        = request.request.session_perso.session_id_low;
  key_info.data_len = request.request.session_perso.data_len;
  key_info.data_ptr = request.request.session_perso.data_ptr;
  feature           = (mmgsdi_perso_feature_enum_type)
                       request.request.session_perso.feature_name;

  mmgsdi_status               =  mmgsdi_session_perso_activate   (
                                    session_id,
                                    feature,
                                    &key_info,
                                    (mmgsdi_callback_type)
                                      uimdiag_mmgsdi_generic_rsp_cb,
                                    (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SESSION_PERSO_ACTIVATE_CMD
    );

  if(delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG session_perso_activate:Delayed Response error");
    return NULL;
  }

  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_perso.data_ptr);

  return rsp_ptr;
}/* uimdiag_mmgsdi_session_perso_activate_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_PERSO_DEACTIVATE_CMD

DESCRIPTION
  Command handler to build a request to deactivate a personalization feature
  by an external source

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_perso_deactivate_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  int32                              index              = 0;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_perso_client_data_type      key_info;
  mmgsdi_session_id_type             session_id;
  mmgsdi_perso_feature_enum_type     feature;


  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_perso_deactivate_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );
  (void)memset ( (void*)&key_info, 0x00,
                 sizeof(mmgsdi_perso_client_data_type));

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_PERSO_DEACTIVATE_CMD);

   if(uimdiag_status != UIMDIAG_SUCCESS)
   {
    UIMDIAG_MSG_HIGH_0("Session Perso Deactivate: Error in uimdiag_mmgsdi_parse_command");
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                                    UIMDIAG_MMGSDI_SESSION_PERSO_DEACTIVATE_CMD;

  session_id        = request.request.session_perso.session_id_low;
  key_info.data_len = request.request.session_perso.data_len;
  key_info.data_ptr = request.request.session_perso.data_ptr;
  feature           = (mmgsdi_perso_feature_enum_type)
                        request.request.session_perso.feature_name;

  mmgsdi_status     = mmgsdi_session_perso_deactivate(
                        session_id,
                        feature,
                        &key_info,
                        (mmgsdi_callback_type)
                        uimdiag_mmgsdi_generic_rsp_cb,
                        (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SESSION_PERSO_DEACTIVATE_CMD
    );

  if(delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG session_perso_deactivate:Delayed Response error");
    return NULL;
  }

  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_perso.data_ptr);

  return rsp_ptr;
}/* uimdiag_mmgsdi_session_perso_deactivate_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_PERSO_GET_IND_CMD

DESCRIPTION
  Command handler to build a request to retrieve the information about
  personalization Indicators

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_perso_get_ind_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  int32                              index              = 0;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_session_id_type             session_id;
  mmgsdi_perso_feature_ind_enum_type  feature_type;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_perso_get_ind_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_PERSO_GET_INDICATORS_CMD);

   if(uimdiag_status != UIMDIAG_SUCCESS)
   {
     UIMDIAG_MSG_HIGH_0("Session Perso get Indicator: Error in uimdiag_mmgsdi_parse_command");
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                                    UIMDIAG_MMGSDI_SESSION_PERSO_GET_INDICATORS_CMD;

  session_id         = request.request.session_perso_get_ind.session_id_low;
  feature_type       = (mmgsdi_perso_feature_ind_enum_type)
                          request.request.session_perso_get_ind.perso_feat_ind;

  mmgsdi_status      =  mmgsdi_session_perso_get_indicators    (
                          session_id,
                          feature_type,
                          (mmgsdi_callback_type)
                          uimdiag_mmgsdi_generic_rsp_cb,
                          (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SESSION_PERSO_GET_INDICATORS_CMD
    );

  if(delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG session_perso_get_ind:Delayed Response error");
    return NULL;
  }

  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_perso.data_ptr);

  return rsp_ptr;
}/* uimdiag_mmgsdi_session_perso_get_ind_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_PERSO_GET_KEY_CMD

DESCRIPTION
  Command handler to build a request to retrieve the information about
  Perso feature Indicator key

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_perso_get_key_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  int32                              index              = 0;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_session_id_type             session_id;
  mmgsdi_perso_feature_enum_type     feature_type;
  mmgsdi_perso_get_key_enum_type     key_type;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_perso_get_key_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_PERSO_GET_KEY_CMD);

   if(uimdiag_status != UIMDIAG_SUCCESS)
   {
     UIMDIAG_MSG_HIGH_0("Session Perso GET KEY: Error in uimdiag_mmgsdi_parse_command");
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                                    UIMDIAG_MMGSDI_SESSION_PERSO_GET_KEY_CMD;

  session_id         = request.request.session_perso_get_key.session_id_low;
  feature_type       = (mmgsdi_perso_feature_enum_type)
                          request.request.session_perso_get_key.feature;
  key_type           =  (mmgsdi_perso_get_key_enum_type)
                          request.request.session_perso_get_key.key_type;

  mmgsdi_status      =  mmgsdi_session_perso_get_key    (
                          session_id,
                          feature_type,
                          key_type,
                          (mmgsdi_callback_type)
                          uimdiag_mmgsdi_generic_rsp_cb,
                          (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SESSION_PERSO_GET_KEY_CMD
    );

  if(delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG session_perso_get_key:Delayed Response error");
    return NULL;
  }

  return rsp_ptr;
}/* uimdiag_mmgsdi_session_perso_get_key_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_PERSO_SET_DATA_CMD

DESCRIPTION
  Command handler to build a request to set the personalization data for a
  particular personalization feature

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_perso_set_data_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  int32                              index              = 0;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_session_id_type             session_id;
  mmgsdi_perso_feature_enum_type     feature_type;
  mmgsdi_perso_client_data_type      perso_codes;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_perso_set_data_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );
  (void)memset ((void*)&perso_codes, 0x00,
                 sizeof(mmgsdi_perso_client_data_type));

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_PERSO_SET_DATA_CMD);

   if(uimdiag_status != UIMDIAG_SUCCESS)
   {
     UIMDIAG_MSG_HIGH_0("Session Perso Set Data: Error in uimdiag_mmgsdi_parse_command");
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                                    UIMDIAG_MMGSDI_SESSION_PERSO_SET_DATA_CMD;

  session_id         = request.request.session_perso.session_id_low;
  feature_type       = (mmgsdi_perso_feature_enum_type)
                         request.request.session_perso.feature_name;

  perso_codes.data_len = request.request.session_perso.data_len;
  perso_codes.data_ptr = request.request.session_perso.data_ptr;
  mmgsdi_status        = mmgsdi_session_perso_set_data (
                           session_id,
                           feature_type,
                           &perso_codes,
                           (mmgsdi_callback_type)
                           uimdiag_mmgsdi_generic_rsp_cb,
                           (mmgsdi_client_data_type)index);

  delayed_rsp_status  = uimdiag_mmgsdi_post_parse_commands(
                          &rsp_ptr,
                          rsp_id,
                          uimdiag_status,
                          index,
                          mmgsdi_status,
                          &request,
                          UIMDIAG_MMGSDI_SESSION_PERSO_SET_DATA_CMD
                          );

  if(delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG session_perso_set_data:Delayed Response error");
    return NULL;
  }

  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_perso.data_ptr);

  return rsp_ptr;
}/* uimdiag_mmgsdi_session_perso_set_data_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_PERSO_GET_DATA_CMD

DESCRIPTION
  Command handler to build a request to get the personalization data for a
  particular personalization feature

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_perso_get_data_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  int32                              index              = 0;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_session_id_type             session_id;
  mmgsdi_perso_feature_enum_type     feature_ind;


  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_perso_get_data_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_PERSO_GET_DATA_CMD);

   if(uimdiag_status != UIMDIAG_SUCCESS)
   {
     UIMDIAG_MSG_HIGH_0("Session Perso Get Data: Error in uimdiag_mmgsdi_parse_command");
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                                    UIMDIAG_MMGSDI_SESSION_PERSO_GET_DATA_CMD;

  session_id         = request.request.session_perso.session_id_low;
  feature_ind        = (mmgsdi_perso_feature_enum_type)
                          request.request.session_perso.feature_name;

  mmgsdi_status      =  mmgsdi_session_perso_get_data (
                          session_id,
                          feature_ind,
                          (mmgsdi_callback_type)
                          uimdiag_mmgsdi_generic_rsp_cb,
                          (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SESSION_PERSO_GET_DATA_CMD
    );

  if(delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG session_perso_get_data:Delayed Response error");
    return NULL;
  }

  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_perso.data_ptr);

  return rsp_ptr;
}/* uimdiag_mmgsdi_session_perso_get_data_cmd */



/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_PERSO_UNBLOCK_CMD

DESCRIPTION
  Command handler to build a request to unblock a personalization feature
  by an external source

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_perso_unblock_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  int32                              index              = 0;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_perso_client_data_type      key_info;
  mmgsdi_session_id_type             session_id;
  mmgsdi_perso_feature_enum_type     feature;


  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_perso_unblock_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );
  (void)memset ( (void*)&key_info, 0x00,
                 sizeof(mmgsdi_perso_client_data_type));

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_PERSO_UNBLOCK_CMD);

   if(uimdiag_status != UIMDIAG_SUCCESS)
   {
    UIMDIAG_MSG_HIGH_0("Session Perso Unblock: Error in uimdiag_mmgsdi_parse_command");
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                                    UIMDIAG_MMGSDI_SESSION_PERSO_UNBLOCK_CMD;

  session_id         = request.request.session_perso.session_id_low;
  key_info.data_len  = request.request.session_perso.data_len;
  key_info.data_ptr  = request.request.session_perso.data_ptr;
  feature            = (mmgsdi_perso_feature_enum_type)
                         request.request.session_perso.feature_name;

  mmgsdi_status      = mmgsdi_session_perso_unblock   (
                         session_id,
                         feature,
                         &key_info,
                         (mmgsdi_callback_type)
                         uimdiag_mmgsdi_generic_rsp_cb,
                         (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
                         &rsp_ptr,
                         rsp_id,
                         uimdiag_status,
                         index,
                         mmgsdi_status,
                         &request,
                         UIMDIAG_MMGSDI_SESSION_PERSO_UNBLOCK_CMD);

  if(delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG session_perso_unblock:Delayed Response error");
    return NULL;
  }

  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_perso.data_ptr);

  return rsp_ptr;
}/* uimdiag_mmgsdi_session_perso_unblock_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_DEACT_PROV_CMD

DESCRIPTION
  Command handler to build a request that allows the client to deactivate
  an application.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_deact_prov_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  int32                              index              = 0;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_session_id_type             session_id;


  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_deact_prov_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_DEACT_PROV_CMD);

   if(uimdiag_status != UIMDIAG_SUCCESS)
   {
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                                    UIMDIAG_MMGSDI_SESSION_DEACT_PROV_CMD;

  session_id         = request.request.session_deact_prov.session_id_low;
  mmgsdi_status      =  mmgsdi_session_deactivate_provisioning (
                          session_id,
                          (mmgsdi_callback_type)
                          uimdiag_mmgsdi_generic_rsp_cb,
                          (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SESSION_DEACT_PROV_CMD
    );

  if(delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG session_deact_prov:Delayed Response error");
    return NULL;
  }

  return rsp_ptr;
}/* uimdiag_mmgsdi_session_deact_prov_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_JCDMA_GET_CARD_INFO_CMD

DESCRIPTION
  Command handler to build a request to MMGSDI for the JCDMA card information.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_jcdma_get_card_info_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  int32                              index              = 0;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_session_id_type             session_id;


  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_jcdma_get_card_info_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_JCDMA_GET_CARD_INFO_CMD);

   if(uimdiag_status != UIMDIAG_SUCCESS)
   {
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                                    UIMDIAG_MMGSDI_SESSION_JCDMA_GET_CARD_INFO_CMD;

  session_id                  =
                        request.request.session_jcdma_get_card_info.session_id_low;

  mmgsdi_status               =   mmgsdi_session_jcdma_get_card_info  (
                                    session_id,
                                    (mmgsdi_callback_type)
                                    uimdiag_mmgsdi_generic_rsp_cb,
                                    (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SESSION_JCDMA_GET_CARD_INFO_CMD
    );

  if(delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG session_jcdma_get_card_info:Delayed Response error");
    return NULL;
  }

  return rsp_ptr;
}/* uimdiag_mmgsdi_session_jcdma_get_card_info_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_GET_APP_CAP_CMD

DESCRIPTION
  Command handler to build a request that allows the client to get application
  capabilities.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_get_app_cap_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  int32                              index              = 0;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_session_id_type             session_id;


  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_get_app_cap_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_GET_APP_CAP_CMD);

   if(uimdiag_status != UIMDIAG_SUCCESS)
   {
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                                    UIMDIAG_MMGSDI_SESSION_GET_APP_CAP_CMD;

  session_id         = request.request.session_get_app_cap.session_id_low;
  mmgsdi_status      = mmgsdi_session_get_app_capabilities(
                         session_id,
                         (mmgsdi_callback_type)
                         uimdiag_mmgsdi_generic_rsp_cb,
                         (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SESSION_GET_APP_CAP_CMD
    );

  if(delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG session_get_app_cap:Delayed Response error");
    return NULL;
  }

  return rsp_ptr;
}/* uimdiag_mmgsdi_session_get_app_cap_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_GET_CPHS_INFO_CMD

DESCRIPTION
  Command handler to build a request to get all CPHS related information

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_get_cphs_info_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  int32                              index              = 0;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_session_id_type             session_id;


  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_get_cphs_info_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_GET_CPHS_INFO_CMD);

   if(uimdiag_status != UIMDIAG_SUCCESS)
   {
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                                    UIMDIAG_MMGSDI_SESSION_GET_CPHS_INFO_CMD;

  session_id         = request.request.session_get_cphs_info.session_id_low;
  mmgsdi_status      = mmgsdi_session_get_cphs_info(
                         session_id,
                         (mmgsdi_callback_type)
                         uimdiag_mmgsdi_generic_rsp_cb,
                         (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SESSION_GET_CPHS_INFO_CMD
    );

  if(delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG session_get_cphs_info:Delayed Response error");
    return NULL;
  }

  return rsp_ptr;
}/* uimdiag_mmgsdi_session_get_cphs_info_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_ISIM_AUTH_CMD

DESCRIPTION
  Command handler for ISIM Authenticate

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_isim_auth_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  int32                              index              = 0;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_session_id_type             session_id;
  mmgsdi_data_type                   auth_req;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_isim_auth_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );
  (void)memset ((void*)&auth_req, 0x00,
                   sizeof(mmgsdi_data_type));

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                       cmd_ptr,
                       pkt_len,
                       &rsp_ptr,
                       &rsp_id,
                       &index,
                       &request,
                       UIMDIAG_MMGSDI_SESSION_ISIM_AUTH_CMD);

  if(uimdiag_status != UIMDIAG_SUCCESS)
  {
    return rsp_ptr;
  }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd = UIMDIAG_MMGSDI_SESSION_ISIM_AUTH_CMD;

  session_id         = request.request.session_isim_auth.session_id_low;

  auth_req.data_len  = request.request.session_isim_auth.auth_data_len;

  UIMDIAG_UTIL_TMC_MEM_MALLOC_AND_VALIDATE(
      auth_req.data_ptr,
      auth_req.data_len);
  if(auth_req.data_ptr == NULL)
  {
    return rsp_ptr;
  }

  if(auth_req.data_len != 0 &&
     auth_req.data_ptr != NULL)
  {
    memscpy(auth_req.data_ptr,
    auth_req.data_len,
    request.request.session_isim_auth.auth_data_ptr,
    request.request.session_isim_auth.auth_data_len);
  }
  else
  {
    UIMDIAG_MSG_ERROR_0("request.request.session_isim_auth data_len is zero");
  }

  mmgsdi_status = mmgsdi_session_isim_authenticate (
                    session_id,
                    auth_req,
                    (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                    (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
                         &rsp_ptr,
                         rsp_id,
                         uimdiag_status,
                         index,
                         mmgsdi_status,
                         &request,
                         UIMDIAG_MMGSDI_SESSION_ISIM_AUTH_CMD);

  if( delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG ISIM Authenticate: Delayed Response error");
    return NULL;
  }

  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_isim_auth.auth_data_ptr);
  return rsp_ptr;
}/* uimdiag_mmgsdi_session_isim_auth_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_SEND_CARD_STATUS_CMD

DESCRIPTION
  Command handler for getting content of current directory

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_send_card_status_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  int32                              index              = 0;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_session_id_type             session_id;
  mmgsdi_status_me_app_enum_type     me_app_status;
  mmgsdi_status_ret_data_enum_type   ret_data_struct;


  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_send_card_status_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_SEND_CARD_STATUS_CMD);

   if(uimdiag_status != UIMDIAG_SUCCESS)
   {
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                                    UIMDIAG_MMGSDI_SESSION_SEND_CARD_STATUS_CMD;

  session_id      = request.request.session_send_crd_status.session_id_low;
  me_app_status   = (mmgsdi_status_me_app_enum_type)
                      request.request.session_send_crd_status.me_app_status;
  ret_data_struct = (mmgsdi_status_ret_data_enum_type)
                      request.request.session_send_crd_status.ret_data_struct;



   mmgsdi_status = mmgsdi_session_send_card_status (
                    session_id,
                    me_app_status,
                    ret_data_struct,
                    (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                    (mmgsdi_client_data_type)index);

   delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SESSION_SEND_CARD_STATUS_CMD);

  if( delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG SEND CARD STATUS: Delayed Response error");
    return NULL;
  }

  return rsp_ptr;
}/* uimdiag_mmgsdi_session_send_card_status_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_DISABLE_SERVICE_CMD

DESCRIPTION
  Command handler for disabling a particular service

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_disable_service_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  int32                              index              = 0;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_service_enum_type           service            = MMGSDI_NONE;
  mmgsdi_session_id_type             session_id;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_disable_service_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_DISABLE_SERVICE_CMD);

   if(uimdiag_status != UIMDIAG_SUCCESS)
   {
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                                    UIMDIAG_MMGSDI_SESSION_DISABLE_SERVICE_CMD;

  session_id         = request.request.session_disable_srv.session_id_low;
  service            = (mmgsdi_service_enum_type)
                         request.request.session_disable_srv.service;

   mmgsdi_status = mmgsdi_session_disable_service (
                    session_id,
                    service,
                    (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                    (mmgsdi_client_data_type)index);

   delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SESSION_DISABLE_SERVICE_CMD);

  if( delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG Session Disable Service: Delayed Response error");
    return NULL;
  }

  return rsp_ptr;
}/* uimdiag_mmgsdi_session_disable_service_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_ENABLE_SERVICE_CMD

DESCRIPTION
  Command handler for enable service

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_enable_service_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  int32                              index              = 0;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_service_enum_type           service            = MMGSDI_NONE;
  mmgsdi_session_id_type             session_id;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_enable_service_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_ENABLE_SERVICE_CMD);

   if(uimdiag_status != UIMDIAG_SUCCESS)
   {
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                                    UIMDIAG_MMGSDI_SESSION_ENABLE_SERVICE_CMD;

  session_id         = request.request.session_enable_srv.session_id_low;
  service            = (mmgsdi_service_enum_type)
                         request.request.session_enable_srv.service;

  mmgsdi_status = mmgsdi_session_enable_service (
                    session_id,
                    service,
                    (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                    (mmgsdi_client_data_type)index);

   delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SESSION_ENABLE_SERVICE_CMD);

  if( delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG Session Enable Service: Delayed Response error");
    return NULL;
  }

  return rsp_ptr;
}/* uimdiag_mmgsdi_session_enable_service_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_IS_SERVICE_AVAIL_CMD

DESCRIPTION
  Command handler to determine if service is available or not.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_is_service_avail_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  int32                              index              = 0;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_service_enum_type           service_type       = MMGSDI_NONE;
  mmgsdi_session_id_type             session_id;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_is_service_available_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_IS_SERVICE_AVAIL_CMD);

   if(uimdiag_status != UIMDIAG_SUCCESS)
   {
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                                    UIMDIAG_MMGSDI_SESSION_IS_SERVICE_AVAIL_CMD;

  session_id         = request.request.session_is_srv_avail.session_id_low;
  service_type       = (mmgsdi_service_enum_type)
                         request.request.session_is_srv_avail.service;

  mmgsdi_status = mmgsdi_session_is_service_available (
                    session_id,
                    service_type,
                    (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                    (mmgsdi_client_data_type)index);

   delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SESSION_IS_SERVICE_AVAIL_CMD);

  if( delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG Session Is Service Available: Delayed Response error");
    return NULL;
  }

  return rsp_ptr;
}/* uimdiag_mmgsdi_session_is_service_avail_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_PERSO_GET_DCK_RETRIES_CMD

DESCRIPTION
  Command handler to determine to get DCK number of Retries.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_perso_get_dck_retries_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  int32                              index              = 0;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_perso_dck_retries_enum_type dck_retries_type   =
                                       MMGSDI_MAX_PERSO_DCK_RETRIES_ENUM;
  mmgsdi_session_id_type             session_id;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_perso_get_dck_retries_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_PERSO_GET_DCK_RETRIES_CMD);

   if(uimdiag_status != UIMDIAG_SUCCESS)
   {
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                                    UIMDIAG_MMGSDI_SESSION_PERSO_GET_DCK_RETRIES_CMD;

  session_id          = request.request.session_perso_get_dck_retries.session_id_low;
  dck_retries_type    = (mmgsdi_perso_dck_retries_enum_type)
                          request.request.session_perso_get_dck_retries.dck_retries_type;

  mmgsdi_status       = mmgsdi_session_perso_get_dck_retries (
                          session_id,
                          dck_retries_type,
                          (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                          (mmgsdi_client_data_type)index);

   delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
                          &rsp_ptr,
                          rsp_id,
                          uimdiag_status,
                          index,
                          mmgsdi_status,
                          &request,
                          UIMDIAG_MMGSDI_SESSION_PERSO_GET_DCK_RETRIES_CMD);

  if( delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG Session Perso Get Dck Retries: Delayed Response error");
    return NULL;
  }

  return rsp_ptr;
}/* uimdiag_mmgsdi_session_perso_get_dck_retries_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_PERSO_LOCK_DWN_ENABLE_LTD_ACCESS_CMD

DESCRIPTION
  Command handler to determine identifiers used for each personalization feature.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_perso_lock_dwn_enable_ltd_access_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  int32                              index              = 0;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_perso_lock_down_enum_type   lock_command       =
                                       MMGSDI_PERSO_LOCK_DOWN_MAX;
  mmgsdi_session_id_type             session_id;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_perso_lock_dwn_enable_ltd_access_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_PERSO_LOCK_DWN_ENABLE_LTD_ACCESS_CMD);

  if(uimdiag_status != UIMDIAG_SUCCESS)
  {
    return rsp_ptr;
  }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                                    UIMDIAG_MMGSDI_SESSION_PERSO_LOCK_DWN_ENABLE_LTD_ACCESS_CMD;

  session_id         = request.request.session_perso_get_dck_retries.session_id_low;
  lock_command       = (mmgsdi_perso_lock_down_enum_type)
                         request.request.session_perso_lock_dwn_enable_ltd_access.action_locked;

  mmgsdi_status      = mmgsdi_session_perso_lock_down_and_enable_limited_access (
                         session_id,
                         lock_command,
                         (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                         (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
                         &rsp_ptr,
                         rsp_id,
                         uimdiag_status,
                         index,
                         mmgsdi_status,
                         &request,
                         UIMDIAG_MMGSDI_SESSION_PERSO_LOCK_DWN_ENABLE_LTD_ACCESS_CMD);

  if(delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG Session Perso Lck Dwn & enable Ltd access: Delayed Response error");
    return NULL;
  }

  return rsp_ptr;
}/* uimdiag_mmgsdi_session_perso_lock_dwn_enable_ltd_access_cmd */

/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_SESSION_SEEK_CMD

DESCRIPTION
  Command handler to perform GSM Seek command on card

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_seek_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr             = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id              = 0;
  uimdiag_return_enum_type           uimdiag_status      = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status  = UIMDIAG_ERROR;
  int32                              index               = 0;
  mmgsdi_return_enum_type            mmgsdi_status       = MMGSDI_ERROR;

  mmgsdi_session_id_type             session_id;
  mmgsdi_access_type                 file_access;
  mmgsdi_seek_direction_enum_type    seek_direction;
  mmgsdi_seek_data_type              seek_pattern;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_seek_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type));
  (void)memset((void*)&file_access, 0x00, sizeof(mmgsdi_access_type));
  (void)memset((void*)&seek_pattern, 0x00, sizeof(mmgsdi_seek_data_type));

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_SEEK_CMD);

  if(uimdiag_status != UIMDIAG_SUCCESS)
  {
    return rsp_ptr;
  }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd = UIMDIAG_MMGSDI_SESSION_SEEK_CMD;

  session_id = request.request.session_seek_req.session_id_low;

  file_access.access_method   = (mmgsdi_access_method_enum_type)
                                   request.request.session_seek_req.access_method;

  switch(file_access.access_method)
  {
   case MMGSDI_EF_ENUM_ACCESS:
        file_access.file.file_enum = uimdiag_util_convert_to_mmgsdi_items_enum(
                                       request.request.session_seek_req.ef_enum);
        break;

   case MMGSDI_DF_ENUM_ACCESS:
        file_access.file.df_enum   = (mmgsdi_df_enum_type)
                                      request.request.session_seek_req.df_enum;
        break;

   case MMGSDI_BY_PATH_ACCESS:
        file_access.file.path_type.path_len = request.request.session_seek_req.path_len;
        if ( (request.request.session_seek_req.path_len > 0) &&
             (request.request.session_seek_req.path_len <= MMGSDI_MAX_PATH_LEN) )
        {
          memscpy(file_access.file.path_type.path_buf,
            sizeof(file_access.file.path_type.path_buf),
            request.request.session_seek_req.path_ptr,
            (request.request.session_seek_req.path_len*sizeof(uint16)));
        }
        else
        {
          UIMDIAG_MSG_ERROR_0("path_len could be <= 0 or exceeds max path len size ");
          return NULL;
        }
        break;

   case MMGSDI_BY_APP_ID_ACCESS:
        file_access.file.app_id.data_len = request.request.session_seek_req.aid_len;
        if (request.request.session_seek_req.aid_len != 0 )
        {
          memscpy(file_access.file.app_id.data_ptr,
            sizeof(file_access.file.app_id.data_ptr),
            request.request.session_seek_req.aid_ptr,
            request.request.session_seek_req.aid_len);
        }
        break;

   default:
        break;
  }

  seek_direction        = (mmgsdi_seek_direction_enum_type)
                            request.request.session_seek_req.seek_direction;

  seek_pattern.data_len = uint32toint32
                           (request.request.session_seek_req.seek_pattern_len);

  if( seek_pattern.data_len != 0)
  {
    UIMDIAG_UTIL_TMC_MEM_MALLOC_AND_VALIDATE(
      seek_pattern.data_ptr,seek_pattern.data_len);

    if(seek_pattern.data_ptr != NULL)
    {
      memscpy(seek_pattern.data_ptr,
        seek_pattern.data_len,
        request.request.session_seek_req.seek_pattern_ptr,
        request.request.session_seek_req.seek_pattern_len);
    }
    else
    {
      return NULL;
    }
  }

  mmgsdi_status      = mmgsdi_session_seek (
                         session_id,
                         file_access,
                         seek_direction,
                         seek_pattern,
                         (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                         (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
                         &rsp_ptr,
                         rsp_id,
                         uimdiag_status,
                         index,
                         mmgsdi_status,
                         &request,
                         UIMDIAG_MMGSDI_SESSION_SEEK_CMD);

  if(delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG session_seek:Delayed Response error");
    return NULL;
  }

  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_seek_req.path_ptr);
  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_seek_req.aid_ptr);
  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_seek_req.seek_pattern_ptr);
  return rsp_ptr;
}/* uimdiag_mmgsdi_session_seek_cmd */

/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_SESSION_SEARCH_CMD

DESCRIPTION
  Command handler to performs Search command on UICC card.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_search_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr             = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id              = 0;
  uimdiag_return_enum_type           uimdiag_status      = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status  = UIMDIAG_ERROR;
  int32                              index               = 0;
  mmgsdi_return_enum_type            mmgsdi_status       = MMGSDI_ERROR;

  mmgsdi_session_id_type             session_id;
  mmgsdi_access_type                 file_access;
  mmgsdi_search_enum_type            search_type;
  mmgsdi_rec_num_type                search_record_num;
  mmgsdi_search_direction_enum_type  search_direction;
  mmgsdi_search_offset_data_type     enhanced_search_offset_data;
  mmgsdi_search_data_type            search_pattern;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_search_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type));
  (void)memset((void*)&file_access, 0x00, sizeof(mmgsdi_access_type));
  (void)memset((void*)&search_pattern, 0x00, sizeof(mmgsdi_search_data_type));

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_SEARCH_CMD);

  if(uimdiag_status != UIMDIAG_SUCCESS)
  {
    UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_search_req.path_ptr);
    UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_search_req.aid_ptr);
    UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_search_req.search_pattern_ptr);
    return rsp_ptr;
  }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd = UIMDIAG_MMGSDI_SESSION_SEARCH_CMD;

  session_id                  = request.request.session_search_req.session_id_low;

  file_access.access_method   = (mmgsdi_access_method_enum_type)
                                   request.request.session_search_req.access_method;

  switch(file_access.access_method)
  {
   case MMGSDI_EF_ENUM_ACCESS:
        file_access.file.file_enum = uimdiag_util_convert_to_mmgsdi_items_enum(
                                       request.request.session_search_req.ef_enum);
        break;

   case MMGSDI_DF_ENUM_ACCESS:
        file_access.file.df_enum   = (mmgsdi_df_enum_type)
                                      request.request.session_search_req.df_enum;
        break;

   case MMGSDI_BY_PATH_ACCESS:
        file_access.file.path_type.path_len = request.request.session_search_req.path_len;
        if ( (request.request.session_search_req.path_len > 0) &&
             (request.request.session_search_req.path_len <= MMGSDI_MAX_PATH_LEN) )
        {
          memscpy(file_access.file.path_type.path_buf,
            sizeof(file_access.file.path_type.path_buf),
            request.request.session_search_req.path_ptr,
            (request.request.session_search_req.path_len*sizeof(uint16)));
        }
        else
        {
          UIMDIAG_MSG_ERROR_0("path_len could be <= 0 or exceeds max path len size ");
          UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_search_req.path_ptr);
          UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_search_req.aid_ptr);
          UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_search_req.search_pattern_ptr);
          return NULL;
        }
        break;

   case MMGSDI_BY_APP_ID_ACCESS:
        file_access.file.app_id.data_len = request.request.session_search_req.aid_len;
        if (request.request.session_search_req.aid_len != 0 )
        {
          memscpy(file_access.file.app_id.data_ptr,
            sizeof(file_access.file.app_id.data_ptr),
            request.request.session_search_req.aid_ptr,
            request.request.session_search_req.aid_len);
        }
        break;

   default:
        break;
  }

  search_type       = (mmgsdi_search_enum_type)
                        request.request.session_search_req.search_type;

  search_record_num = uint32toint32(
                        request.request.session_search_req.search_record_num);

  search_direction  = (mmgsdi_search_direction_enum_type)
                        request.request.session_search_req.search_direction;

  enhanced_search_offset_data.offset_type   = (mmgsdi_search_option_enum_type)
                    request.request.session_search_req.offset_type;

  enhanced_search_offset_data.offset_data = uint32toint32(
                                              request.request.session_search_req.offset_data);

  search_pattern.data_len = request.request.session_search_req.search_pattern_len;

  UIMDIAG_UTIL_TMC_MEM_MALLOC_AND_VALIDATE(
      search_pattern.data_ptr,search_pattern.data_len);

  if(search_pattern.data_ptr != NULL && search_pattern.data_len != 0 &&
       request.request.session_search_req.search_pattern_ptr != NULL)
  {
    memscpy(search_pattern.data_ptr,
      search_pattern.data_len,
      request.request.session_search_req.search_pattern_ptr,
      request.request.session_search_req.search_pattern_len);
  }

  mmgsdi_status      = mmgsdi_session_search(
                         session_id,
                         file_access,
                         search_type,
                         search_record_num,
                         search_direction,
                         enhanced_search_offset_data,
                         search_pattern,
                         (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                         (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
                         &rsp_ptr,
                         rsp_id,
                         uimdiag_status,
                         index,
                         mmgsdi_status,
                         &request,
                         UIMDIAG_MMGSDI_SESSION_SEARCH_CMD);

  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_search_req.path_ptr);
  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_search_req.aid_ptr);
  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_search_req.search_pattern_ptr);
  UIMDIAGUTIL_TMC_MEM_FREE(search_pattern.data_ptr);
  if(delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG session_search: Delayed Response error");
    return NULL;
  }

  return rsp_ptr;
}/* uimdiag_mmgsdi_session_search_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_PROV_APP_INIT_COMPLETE_CMD

DESCRIPTION
  Notifies the MMGSDI that the client has finished its initialization procedure
  for the application.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_prov_app_init_complete_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  int32                              index              = 0;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_session_id_type             session_id;
  boolean                            init_status;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_prov_app_init_complete_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_PROV_APP_INIT_COMPLETE_CMD);

   if( uimdiag_status != UIMDIAG_SUCCESS)
   {
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
    UIMDIAG_MMGSDI_SESSION_PROV_APP_INIT_COMPLETE_CMD;

  session_id        =
    request.request.session_prov_app_init_complete.session_id_low;
  init_status       =
    (mmgsdi_service_enum_type)
      request.request.session_prov_app_init_complete.init_status;

  mmgsdi_status = mmgsdi_session_prov_app_init_complete (
                    session_id,
                    init_status,
                    (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                    (mmgsdi_client_data_type)index);

   delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SESSION_PROV_APP_INIT_COMPLETE_CMD);

  if( delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG Session Prov App Init Complete : Delayed Response error");
    return NULL;
  }

  return rsp_ptr;
}/* uimdiag_mmgsdi_session_prov_app_init_complete_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_COMPUTE_IP_AUTH_CMD

DESCRIPTION
  compute IP  authentication for 3GPD SIMPLE IP CHAP, Mobile IP, or HRPD CHAP
  Authentication.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_compute_ip_auth_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  int32                              index              = 0;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_compute_ip_data_type        compute_ip_data;
  mmgsdi_session_id_type             session_id;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_compute_ip_auth_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_COMPUTE_IP_AUTH_CMD);

   if( uimdiag_status != UIMDIAG_SUCCESS)
   {
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
    UIMDIAG_MMGSDI_SESSION_COMPUTE_IP_AUTH_CMD;

  session_id         = request.request.session_compute_ip_auth.session_id_low;
  compute_ip_data.operation_type =
    (mmgsdi_compute_ip_operation_enum_type)
      request.request.session_compute_ip_auth.operation_type;
  switch( compute_ip_data.operation_type)
  {
    case MMGSDI_CMPT_IP_SIP_CHAP:
      compute_ip_data.data.sip_chap_data.chap_id =
        request.request.session_compute_ip_auth.chap_id;
      compute_ip_data.data.sip_chap_data.nai_entry_index =
        request.request.session_compute_ip_auth.chap_nai_entry_index;
      compute_ip_data.data.sip_chap_data.chap_challenge_length =
        request.request.session_compute_ip_auth.chap_chal_len;
      memscpy((compute_ip_data.data.sip_chap_data.chap_challenge),
        sizeof(compute_ip_data.data.sip_chap_data.chap_challenge),
        request.request.session_compute_ip_auth.chap_challenge,
        request.request.session_compute_ip_auth.chap_chal_len);
      break;
    case MMGSDI_CMPT_IP_MIP_MN_HA:
      compute_ip_data.data.mn_ha_data.nai_entry_index =
        request.request.session_compute_ip_auth.mn_ha_nai_entry_index;
      compute_ip_data.data.mn_ha_data.registration_data_length =
        request.request.session_compute_ip_auth.mn_ha_reg_data_len;
      compute_ip_data.data.mn_ha_data.registration_data_ptr =
        request.request.session_compute_ip_auth.mn_ha_reg_data;
      break;
    case MMGSDI_CMPT_IP_MIP_RRQ:
      compute_ip_data.data.rrq_data.rrq_data_length =
        request.request.session_compute_ip_auth.rrq_data_len;
      compute_ip_data.data.rrq_data.rrq_data_ptr =
        request.request.session_compute_ip_auth.rrq_data;
      break;
    case MMGSDI_CMPT_IP_MIP_MN_AAA:
      compute_ip_data.data.mn_aaa_data.nai_entry_index =
        request.request.session_compute_ip_auth.mn_aaa_nai_entry_index;
      compute_ip_data.data.mn_aaa_data.challenge_length =
        request.request.session_compute_ip_auth.mn_aaa_chal_len;
      compute_ip_data.data.mn_aaa_data.challenge_ptr =
        request.request.session_compute_ip_auth.mn_aaa_challenge;
      break;
    case MMGSDI_CMPT_IP_HRPD_CHAP:
      compute_ip_data.data.hrpd_chap_data.chap_id =
        request.request.session_compute_ip_auth.hrpd_chap_id;
      compute_ip_data.data.hrpd_chap_data.chap_challenge_length =
        request.request.session_compute_ip_auth.hrpd_chap_chal_len;
      compute_ip_data.data.hrpd_chap_data.chap_challenge_ptr =
        request.request.session_compute_ip_auth.hrpd_chap_challenge;
      break;
    default:
        UIMDIAG_MSG_ERROR_0("Wrong Compute IP operation type");
    }

  mmgsdi_status = mmgsdi_session_compute_ip_auth(
                    session_id,
                    compute_ip_data,
                    (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                    (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SESSION_COMPUTE_IP_AUTH_CMD);

  uimdiag_mmgsdi_free_cmd_data(&request,
    UIMDIAG_MMGSDI_SESSION_COMPUTE_IP_AUTH_CMD);

  if( delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG Session Compute IP Data: Delayed Response error");
    return NULL;
  }

  return rsp_ptr;
}/* uimdiag_mmgsdi_session_compute_ip_auth_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_SSD_UPDATE_CMD

DESCRIPTION
  Used to initiate the calculation of a new SSD value
                    for authentication purposes.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_ssd_update_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  int32                              index              = 0;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  qword                              randssd;
  byte                               process_control;
  dword                              esn;
  mmgsdi_session_id_type             session_id;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_ssd_update_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_SSD_UPDATE_CMD);

   if( uimdiag_status != UIMDIAG_SUCCESS)
   {
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                                    UIMDIAG_MMGSDI_SESSION_SSD_UPDATE_CMD;

  session_id         = request.request.session_ssd_update.session_id_low;
  randssd[1]         = request.request.session_ssd_update.randssd_high;
  randssd[0]         = request.request.session_ssd_update.randssd_low;
  esn                = request.request.session_ssd_update.esn;
  process_control    = request.request.session_ssd_update.process_control;

  mmgsdi_status = mmgsdi_session_ssd_update (
                    session_id,
                    randssd,
                    process_control,
                    esn,
                    (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                    (mmgsdi_client_data_type)index);

   delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SESSION_SSD_UPDATE_CMD);

  if( delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG Session SSD Update: Delayed Response error");
    return NULL;
  }

  return rsp_ptr;
}/* uimdiag_mmgsdi_session_ssd_update_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_CONFIRM_SSD_CMD

DESCRIPTION
  Performs a Confirm SSD command for authentication.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_confirm_ssd_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  int32                              index              = 0;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  dword                              authbs;
  mmgsdi_session_id_type             session_id;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_confirm_ssd_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_SSD_CONFIRM_CMD);

   if( uimdiag_status != UIMDIAG_SUCCESS)
   {
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                                    UIMDIAG_MMGSDI_SESSION_SSD_CONFIRM_CMD;

  session_id         = request.request.session_cnf_ssd.session_id_low;
  authbs             = (dword)request.request.session_cnf_ssd.authbs;

  mmgsdi_status = mmgsdi_session_confirm_ssd (
                    session_id,
                    authbs,
                    (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                    (mmgsdi_client_data_type)index);

   delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SESSION_SSD_CONFIRM_CMD);

  if( delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG Session Confirm SSD: Delayed Response error");
    return NULL;
  }

  return rsp_ptr;
}/* uimdiag_mmgsdi_session_confirm_ssd_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_BS_CHAL_CMD

DESCRIPTION
  Used to perform an OTASP base station challenge before
  updating its SSD to ensure that the update process is being initiated by a
  legitimate source

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_bs_chal_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  int32                              index              = 0;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  dword                              randseed;
  mmgsdi_session_id_type             session_id;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_bs_chal_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_BS_CHAL_CMD);

   if( uimdiag_status != UIMDIAG_SUCCESS)
   {
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                                    UIMDIAG_MMGSDI_SESSION_BS_CHAL_CMD;

  session_id         = request.request.session_bs_chal.session_id_low;
  randseed           = (dword)request.request.session_bs_chal.randseed;

  mmgsdi_status = mmgsdi_session_bs_chal(
                    session_id,
                    randseed,
                    (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                    (mmgsdi_client_data_type)index);

   delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SESSION_BS_CHAL_CMD);

  if( delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG Session BS Challenge: Delayed Response error");
    return NULL;
  }

  return rsp_ptr;
}/* uimdiag_mmgsdi_session_bs_chal_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_CHECK_DIAG_SUPPORT_CMD

DESCRIPTION
  Gets the IS-683-C or IS-683-A PRL from either the EF Extended
  PRL (EF-EPRL) or the EF-PRL when the EF-EPRL does not contain a valid PRL.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_read_prl_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  int32                              index              = 0;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_session_id_type             session_id;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_read_prl_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_READ_PRL_CMD);

   if( uimdiag_status != UIMDIAG_SUCCESS)
   {
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                                    UIMDIAG_MMGSDI_SESSION_READ_PRL_CMD;

  session_id         = request.request.session_read_prl.session_id_low;

  mmgsdi_status = mmgsdi_session_read_prl(
                    session_id,
                    (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                    (mmgsdi_client_data_type)index);

   delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SESSION_READ_PRL_CMD);

  if( delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG Session Read PRL: Delayed Response error");
    return NULL;
  }

  return rsp_ptr;
}/* uimdiag_mmgsdi_session_read_prl_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_GET_INFO_SYNC_CMD

DESCRIPTION
  Command handler to determine if service is available or not.

DEPENDENCIES
  None.

RETURN VALUE
# if value is 1 , then uimdiag is supported, if value is 2, GSDIDIAG is supported.
# Preference and by default behaviour is UIMDIAG support.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_check_diag_support_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_check_diag_support_rsp_type          *rsp_ptr            = NULL;

  UIMDIAG_MSG_HIGH_0("uimdiag_check_diag_support_cmd");

  /*  allocate immediate response pointer  */
  rsp_ptr = (uimdiag_check_diag_support_rsp_type *)
                diagpkt_subsys_alloc_v2(
                  (diagpkt_subsys_id_type) DIAG_SUBSYS_GSDI,
                  (diagpkt_subsys_cmd_code_type) UIMDIAG_MMGSDI_CHECK_DIAG_SUPPORT_CMD,
                  sizeof(uimdiag_check_diag_support_rsp_type));
  if( rsp_ptr == NULL)
  {
    UIMDIAG_MSG_ERROR_0("rsp_ptr == NULL");
    return NULL;
  }
  rsp_ptr->status = 2;
  diagpkt_commit(rsp_ptr);

  return rsp_ptr;
}/* uimdiag_check_diag_support_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_GET_INFO_SYNC_CMD

DESCRIPTION
This function allows a client to query MMGSDI for session information by
Session ID or Session Type. A client can get Session Type, Slot Number,
Channel ID and AID using this request.


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_get_info_sync_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  uimdiag_generic_rsp_type          *delayed_rsp_ptr    = NULL;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_session_info_query_type     session_info_query;
  mmgsdi_session_get_info_type       session_get_info;


  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_get_info_sync_cmd");
  memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );
  memset((void*)&session_info_query, 0x00, sizeof(mmgsdi_session_info_query_type) );

  if( cmd_ptr == NULL)
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL");
    return NULL;
  }

  /* extract data from req packet
   and populate fields in the request structure */
  uimdiag_status = uimdiag_parse_cmd_data(&request, cmd_ptr, pkt_len);

  if( uimdiag_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_1("uimdiag_mmgsdi_parse_commands: Parsing error 0x%x",
                        uimdiag_status);
    return NULL;
  }

  /*  allocate response pointer  */
  rsp_ptr = (uimdiag_generic_rsp_type *)
                diagpkt_subsys_alloc_v2(
                  (diagpkt_subsys_id_type) DIAG_SUBSYS_GSDI,
                  (diagpkt_subsys_cmd_code_type) UIMDIAG_MMGSDI_SESSION_GET_INFO_SYNC_CMD,
                  sizeof(uimdiag_generic_rsp_type));

  if (rsp_ptr == NULL)
  {
    UIMDIAG_MSG_ERROR_0("rsp_ptr == NULL");
    return rsp_ptr;
  }

  rsp_id = diagpkt_subsys_get_delayed_rsp_id(rsp_ptr);
  session_info_query.query_type = (mmgsdi_session_info_enum_type)
      request.request.session_get_info_sync.query_type;
  if( session_info_query.query_type == MMGSDI_SESSION_INFO_QUERY_BY_ID)
  {
    session_info_query.query_key.session_id =
      request.request.session_get_info_sync.session_id_low;
  }
  else if( session_info_query.query_type == MMGSDI_SESSION_INFO_QUERY_BY_TYPE)
  {
    session_info_query.query_key.session_type =
      (mmgsdi_session_type_enum_type)
        request.request.session_get_info_sync.session_type;
  }
  mmgsdi_status = mmgsdi_session_get_info_sync(
                    session_info_query,
                    &session_get_info);

  diagpkt_subsys_set_status(rsp_ptr, (uint32) UIMDIAG_SUCCESS);
  diagpkt_commit(rsp_ptr);

  /* allocate delayed response pointer */
  delayed_rsp_ptr = (uimdiag_generic_rsp_type *)
              diagpkt_subsys_alloc_v2_delay(
              (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
              (diagpkt_subsys_cmd_code_type) UIMDIAG_MMGSDI_SESSION_GET_INFO_SYNC_CMD,
              rsp_id,
              sizeof(uimdiag_generic_rsp_type));

  if (delayed_rsp_ptr == NULL)
  {
    return NULL;
  }

  (void)memset((void*)&delayed_rsp_ptr->rsp_payload,0x00,sizeof(uimdiag_cmd_rsp_pyld_type));

  delayed_rsp_ptr->rsp_payload.mmgsdi_status = mmgsdi_status;

  if( mmgsdi_status == MMGSDI_SUCCESS )
  {
    uimdiag_status =
      uimdiag_mmgsdi_session_get_info_sync_cnf(delayed_rsp_ptr,&session_get_info);
  }
  rex_sleep(10);
  diagpkt_delay_commit(delayed_rsp_ptr);
  return NULL;
}/* uimdiag_mmgsdi_session_get_info_sync_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_READ_CACHE_FILE_SIZE_CMD

DESCRIPTION
  Command handler to get the size of the file to be cached for a read operation.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_read_cache_file_size_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  diagpkt_subsys_delayed_rsp_id_type    rsp_id          = 0;
  uimdiag_generic_rsp_type          * delayed_rsp_ptr   = NULL;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_session_id_type             session_id;
  mmgsdi_len_type                    file_size = 0;
  mmgsdi_file_enum_type              file_name;


  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_read_cache_file_size_cmd");
  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if( cmd_ptr == NULL)
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL");
    return NULL;
  }

  /* extract data from req packet
   and populate fields in the request structure */
  uimdiag_status = uimdiag_parse_cmd_data(&request, cmd_ptr, pkt_len);

  if( uimdiag_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_1("uimdiag_mmgsdi_parse_commands: Parsing error 0x%x",
                        uimdiag_status);
    return NULL;
  }
  /*  allocate response pointer  */
  rsp_ptr = (uimdiag_generic_rsp_type *)
                diagpkt_subsys_alloc_v2(
                  (diagpkt_subsys_id_type) DIAG_SUBSYS_GSDI,
                  (diagpkt_subsys_cmd_code_type) UIMDIAG_MMGSDI_SESSION_READ_CACHE_FILE_SIZE_CMD,
                  sizeof(uimdiag_generic_rsp_type));

  if( rsp_ptr == NULL)
  {
    UIMDIAG_MSG_ERROR_0("rsp_ptr == NULL");
    return rsp_ptr;
  }
  rsp_id = diagpkt_subsys_get_delayed_rsp_id(rsp_ptr );

  session_id         =
    request.request.session_cache_file_size_type.session_id_low;
  file_name          =
    uimdiag_util_convert_to_mmgsdi_items_enum(
      request.request.session_cache_file_size_type.file_name);
  mmgsdi_status      = mmgsdi_session_read_cache_file_size(
                         session_id,
                         file_name,
                         &file_size);

  diagpkt_subsys_set_status(rsp_ptr, (uint32) UIMDIAG_SUCCESS);
  diagpkt_commit(rsp_ptr);

  /* allocate delayed response pointer */
  delayed_rsp_ptr = (uimdiag_generic_rsp_type *)
              diagpkt_subsys_alloc_v2_delay(
              (diagpkt_subsys_id_type) DIAG_SUBSYS_GSDI,
              (diagpkt_subsys_cmd_code_type) UIMDIAG_MMGSDI_SESSION_READ_CACHE_FILE_SIZE_CMD,
              rsp_id,
              sizeof(uimdiag_generic_rsp_type));

  if (delayed_rsp_ptr == NULL)
  {
    UIMDIAG_MSG_ERROR_0("delayed_rsp_ptr == NULL");
    return NULL;
  }

  (void)memset((void*)&delayed_rsp_ptr->rsp_payload,0x00,sizeof(uimdiag_cmd_rsp_pyld_type));

  delayed_rsp_ptr->rsp_payload.mmgsdi_status = mmgsdi_status;

  if( mmgsdi_status == MMGSDI_SUCCESS )
  {
    delayed_rsp_ptr->rsp_payload.response.session_cache_file_size_rsp.file_size = file_size;
  }
  rex_sleep(10);
  diagpkt_delay_commit(delayed_rsp_ptr);
  return NULL;
}/* uimdiag_mmgsdi_session_read_cache_file_size_cmd */
/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_READ_CACHE_CMD

DESCRIPTION
  Command handler to read the cached file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_read_cache_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  uimdiag_generic_rsp_type          *delayed_rsp_ptr    = NULL;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_session_id_type             session_id;
  uint8                              data[UIMDIAG_MAX_READ_DATA_LEN];
  mmgsdi_data_type                   read_data;
  mmgsdi_file_enum_type              file_name;


  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_read_cache_cmd");
  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if( cmd_ptr == NULL)
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL");
    return NULL;
  }

  /* extract data from req packet
   and populate fields in the request structure */
  uimdiag_status = uimdiag_parse_cmd_data(&request, cmd_ptr, pkt_len);

  if( uimdiag_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_1("uimdiag_mmgsdi_parse_commands: Parsing error 0x%x",
                        uimdiag_status);
    return NULL;
  }
  /*  allocate immediate response pointer  */
  rsp_ptr = (uimdiag_generic_rsp_type *)
                diagpkt_subsys_alloc_v2(
                  (diagpkt_subsys_id_type) DIAG_SUBSYS_GSDI,
                  (diagpkt_subsys_cmd_code_type) UIMDIAG_MMGSDI_SESSION_READ_CACHE_CMD,
                  sizeof(uimdiag_generic_rsp_type));

  if( rsp_ptr == NULL)
  {
    UIMDIAG_MSG_ERROR_0("rsp_ptr == NULL");
    return rsp_ptr;
  }

  rsp_id = diagpkt_subsys_get_delayed_rsp_id(rsp_ptr);

  session_id         =
    request.request.session_cache_file_size_type.session_id_low;
  read_data.data_len = request.request.session_read_cache.data_len;
  read_data.data_ptr = &data[0];
  file_name          = uimdiag_util_convert_to_mmgsdi_items_enum(
                         request.request.session_read_cache.file_name);
  mmgsdi_status      = mmgsdi_session_read_cache(
                         session_id,
                         file_name,
                         read_data);

  diagpkt_subsys_set_status(rsp_ptr, (uint32) UIMDIAG_SUCCESS);
  diagpkt_commit(rsp_ptr);

  /* allocate delayed response pointer */
  delayed_rsp_ptr = (uimdiag_generic_rsp_type *)
              diagpkt_subsys_alloc_v2_delay(
              (diagpkt_subsys_id_type)DIAG_SUBSYS_UIM,
              (diagpkt_subsys_cmd_code_type) UIMDIAG_MMGSDI_SESSION_READ_CACHE_CMD,
              rsp_id,
              sizeof(uimdiag_generic_rsp_type));

  if (delayed_rsp_ptr == NULL)
  {
    UIMDIAG_MSG_ERROR_0("delayed_rsp_ptr == NULL");
    return NULL;
  }

  (void)memset((void*)&delayed_rsp_ptr->rsp_payload,0x00,sizeof(uimdiag_cmd_rsp_pyld_type));

  delayed_rsp_ptr->rsp_payload.mmgsdi_status = mmgsdi_status;

  if( mmgsdi_status == MMGSDI_SUCCESS )
  {
    uimdiag_status =
      uimdiag_mmgsdi_session_read_cache_cnf(delayed_rsp_ptr,&read_data);
  }
  rex_sleep(10);
  diagpkt_delay_commit(delayed_rsp_ptr);
  return NULL;
}/* uimdiag_mmgsdi_session_read_cache_cmd */
/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_WRITE_CACHE_CMD

DESCRIPTION
  Command handler to write data to a cached file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_write_cache_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  uimdiag_generic_rsp_type          *delayed_rsp_ptr    =  NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_session_id_type             session_id;
  mmgsdi_data_type                   write_data;
  mmgsdi_file_enum_type              file_name;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_write_cache_cmd");
  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if( cmd_ptr == NULL)
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL");
    return NULL;
  }

  /* extract data from req packet
   and populate fields in the request structure */
  uimdiag_status = uimdiag_parse_cmd_data(&request, cmd_ptr, pkt_len);

  if( uimdiag_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_1("uimdiag_mmgsdi_parse_commands: Parsing error 0x%x",
                        uimdiag_status);
    uimdiag_mmgsdi_free_cmd_data(&request, UIMDIAG_MMGSDI_SESSION_WRITE_CACHE_CMD);
    return NULL;
  }

  /*  allocate response pointer  */
  rsp_ptr = (uimdiag_generic_rsp_type *)
                diagpkt_subsys_alloc_v2(
                  (diagpkt_subsys_id_type) DIAG_SUBSYS_GSDI,
                  (diagpkt_subsys_cmd_code_type) UIMDIAG_MMGSDI_SESSION_WRITE_CACHE_CMD,
                  sizeof(uimdiag_generic_rsp_type));

  if( rsp_ptr == NULL)
  {
    UIMDIAG_MSG_ERROR_0("rsp_ptr == NULL");
    uimdiag_mmgsdi_free_cmd_data(&request, UIMDIAG_MMGSDI_SESSION_WRITE_CACHE_CMD);
    return rsp_ptr;
  }

  rsp_id = diagpkt_subsys_get_delayed_rsp_id(rsp_ptr );
  session_id          = request.request.session_write_cache.session_id_low;
  write_data.data_len = request.request.session_write_cache.data_len;
  write_data.data_ptr = request.request.session_write_cache.data;
  file_name           = uimdiag_util_convert_to_mmgsdi_items_enum(
                          request.request.session_write_cache.file_name);
  mmgsdi_status       = mmgsdi_session_write_cache(
                          session_id,
                          file_name,
                          write_data);

  diagpkt_subsys_set_status(rsp_ptr, (uint32) UIMDIAG_SUCCESS);
  diagpkt_commit(rsp_ptr);

  /* allocate delayed response pointer */
  delayed_rsp_ptr = (uimdiag_generic_rsp_type *)
           diagpkt_subsys_alloc_v2_delay(
             (diagpkt_subsys_id_type)DIAG_SUBSYS_UIM,
             (diagpkt_subsys_cmd_code_type) UIMDIAG_MMGSDI_SESSION_WRITE_CACHE_CMD,
             rsp_id,
             sizeof(uimdiag_generic_rsp_type));

  if (delayed_rsp_ptr == NULL)
  {
    UIMDIAG_MSG_ERROR_0("delayed_rsp_ptr == NULL");
    return NULL;
  }

  (void)memset((void*)&delayed_rsp_ptr->rsp_payload,0x00,
                      sizeof(uimdiag_cmd_rsp_pyld_type));

  delayed_rsp_ptr->rsp_payload.mmgsdi_status = mmgsdi_status;
  diagpkt_subsys_set_status(delayed_rsp_ptr,(uint32)UIMDIAG_SUCCESS);
  rex_sleep(10);
  diagpkt_delay_commit(delayed_rsp_ptr);
  uimdiag_mmgsdi_free_cmd_data(&request,
                                 UIMDIAG_MMGSDI_SESSION_WRITE_CACHE_CMD);
  return NULL;

}/* uimdiag_mmgsdi_session_write_cache_cmd */
/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_ILLEGAL_SUBSCRIPTION_SYNC_CMD

DESCRIPTION
  Command handler to notify UIM that the subscription on the card is illegal and is to be blocked.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_illegal_subscription_sync_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  uimdiag_generic_rsp_type          *delayed_rsp_ptr    =  NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_session_id_type             session_id;


  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_illegal_subscription_sync_cmd");
  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if( cmd_ptr == NULL)
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL");
    return NULL;
  }
  /* extract data from req packet
   and populate fields in the request structure */
  uimdiag_status = uimdiag_parse_cmd_data(&request, cmd_ptr, pkt_len);

  if( uimdiag_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_1("uimdiag_mmgsdi_parse_commands: Parsing error 0x%x",
                        uimdiag_status);
    return NULL;
  }
  /*  allocate response pointer  */
  rsp_ptr = (uimdiag_generic_rsp_type *)
                diagpkt_subsys_alloc_v2(
                  (diagpkt_subsys_id_type) DIAG_SUBSYS_GSDI,
                  (diagpkt_subsys_cmd_code_type) UIMDIAG_MMGSDI_SESSION_ILLEGAL_SUBSCRIPTION_SYNC_CMD,
                  sizeof(uimdiag_generic_rsp_type));
  if( rsp_ptr == NULL)
  {
    UIMDIAG_MSG_ERROR_0("rsp_ptr == NULL");
    return rsp_ptr;
  }

  rsp_id = diagpkt_subsys_get_delayed_rsp_id(rsp_ptr);
  session_id    = request.request.session_illegal_suscr_sync.session_id_low;
  mmgsdi_status = mmgsdi_session_illegal_subscription_sync(
                    session_id);

  diagpkt_subsys_set_status(rsp_ptr, (uint32) UIMDIAG_SUCCESS);
  diagpkt_commit(rsp_ptr);

  /* allocate delayed response pointer */
  delayed_rsp_ptr = (uimdiag_generic_rsp_type *)
              diagpkt_subsys_alloc_v2_delay(
              (diagpkt_subsys_id_type) DIAG_SUBSYS_GSDI,
              (diagpkt_subsys_cmd_code_type) UIMDIAG_MMGSDI_SESSION_ILLEGAL_SUBSCRIPTION_SYNC_CMD,
              rsp_id,
              sizeof(uimdiag_generic_rsp_type));

  if (delayed_rsp_ptr == NULL)
  {
    UIMDIAG_MSG_ERROR_0("delayed_rsp_ptr == NULL");
    return NULL;
  }

  (void)memset((void*)&delayed_rsp_ptr->rsp_payload,0x00,sizeof(uimdiag_cmd_rsp_pyld_type));

  delayed_rsp_ptr->rsp_payload.mmgsdi_status = mmgsdi_status;
  rex_sleep(10);
  diagpkt_delay_commit(delayed_rsp_ptr);
  return NULL;
}/* uimdiag_mmgsdi_session_illegal_subscription_sync_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_TOGGLE_UIM_PWR_CTRL_SYNC_CMD

DESCRIPTION
  Command handler to notify UIM when the modem is in or out of the traffic channel.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_toggle_uim_pwr_ctrl_sync_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  uimdiag_generic_rsp_type          *delayed_rsp_ptr    =  NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_session_id_type             session_id;


  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_toggle_uim_pwr_ctrl_sync_cmd");
  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if( cmd_ptr == NULL)
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL");
    return NULL;
  }

  /* extract data from req packet
   and populate fields in the request structure */
  uimdiag_status = uimdiag_parse_cmd_data(&request, cmd_ptr, pkt_len);

  if( uimdiag_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_1("uimdiag_mmgsdi_parse_commands: Parsing error 0x%x",
                        uimdiag_status);
    return NULL;
  }
  /*  allocate response pointer  */
  rsp_ptr = (uimdiag_generic_rsp_type *)
                diagpkt_subsys_alloc_v2(
                  (diagpkt_subsys_id_type) DIAG_SUBSYS_GSDI,
                  (diagpkt_subsys_cmd_code_type) UIMDIAG_MMGSDI_SESSION_TOGGLE_UIM_PWR_CTRL_SYNC_CMD,
                  sizeof(uimdiag_generic_rsp_type));

  if( rsp_ptr == NULL)
  {
    UIMDIAG_MSG_ERROR_0("rsp_ptr == NULL");
    return NULL;
  }

  rsp_id = diagpkt_subsys_get_delayed_rsp_id(rsp_ptr);
  session_id    =
    request.request.session_toggle_uim_pwr_ctrl.session_id_low;
  mmgsdi_status = mmgsdi_session_toggle_uim_power_control_sync(
                    session_id,
                    request.request.session_toggle_uim_pwr_ctrl.in_TC,
                    (mmgsdi_in_tc_mode_enum_type)
                    request.request.session_toggle_uim_pwr_ctrl.mode,
                    UIM_CALL_TYPE_NOT_APPLICABLE);

  diagpkt_subsys_set_status(rsp_ptr, (uint32) UIMDIAG_SUCCESS);
  diagpkt_commit(rsp_ptr);

  /* allocate delayed response pointer */
  delayed_rsp_ptr = (uimdiag_generic_rsp_type *)
              diagpkt_subsys_alloc_v2_delay(
              (diagpkt_subsys_id_type) DIAG_SUBSYS_GSDI,
              (diagpkt_subsys_cmd_code_type) UIMDIAG_MMGSDI_SESSION_TOGGLE_UIM_PWR_CTRL_SYNC_CMD,
              rsp_id,
              sizeof(uimdiag_generic_rsp_type));

  if (delayed_rsp_ptr == NULL)
  {
    UIMDIAG_MSG_ERROR_0("delayed_rsp_ptr == NULL");
    return NULL;
  }

  (void)memset((void*)&delayed_rsp_ptr->rsp_payload,0x00,sizeof(uimdiag_cmd_rsp_pyld_type));

  delayed_rsp_ptr->rsp_payload.mmgsdi_status = mmgsdi_status;
  rex_sleep(10);
  diagpkt_delay_commit(delayed_rsp_ptr);
  return NULL;
}/* uimdiag_mmgsdi_session_toggle_uim_pwr_ctrl_sync_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_GET_APP_CAPABILITIES_SYNC_CMD

DESCRIPTION
  Command handler to extract all the provisioning application
  capabilities like fdn, bdn, acl, imsi status and phone book related.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_get_app_capabilities_sync_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_session_app_capabilities_cnf_type       app_capabilities_sync;
  mmgsdi_session_id_type             session_id;
  diagpkt_subsys_delayed_rsp_id_type    rsp_id          = 0;
  uimdiag_generic_rsp_type          * delayed_rsp_ptr   = NULL;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_get_app_capabilities_sync_cmd");
  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );
  (void)memset((void*)&app_capabilities_sync, 0x00, sizeof(mmgsdi_session_app_capabilities_cnf_type));

  if( cmd_ptr == NULL)
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL");
    return NULL;
  }

  /* extract data from req packet
   and populate fields in the request structure */
  uimdiag_status = uimdiag_parse_cmd_data(&request, cmd_ptr, pkt_len);

  if( uimdiag_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_1("uimdiag_mmgsdi_parse_commands: Parsing error 0x%x",
                        uimdiag_status);
    return NULL;
  }

  /*  allocate response pointer  */
  rsp_ptr = (uimdiag_generic_rsp_type *)
                diagpkt_subsys_alloc_v2(
                  (diagpkt_subsys_id_type) DIAG_SUBSYS_GSDI,
                  (diagpkt_subsys_cmd_code_type) UIMDIAG_MMGSDI_SESSION_GET_APP_CAPABILITIES_SYNC_CMD,
                  sizeof(uimdiag_generic_rsp_type));

  if( rsp_ptr == NULL)
  {
    UIMDIAG_MSG_ERROR_0("rsp_ptr == NULL");
    return NULL;
  }

  rsp_id = diagpkt_subsys_get_delayed_rsp_id(rsp_ptr );

  session_id    = request.request.session_get_app_cap_sync.session_id_low;
  mmgsdi_status = mmgsdi_session_get_app_capabilities_sync(
                    session_id,
                    &app_capabilities_sync);

  diagpkt_subsys_set_status(rsp_ptr, (uint32) UIMDIAG_SUCCESS);
  diagpkt_commit(rsp_ptr);

/* allocate delayed response pointer */
  delayed_rsp_ptr = (uimdiag_generic_rsp_type *)
              diagpkt_subsys_alloc_v2_delay(
              (diagpkt_subsys_id_type) DIAG_SUBSYS_GSDI,
              (diagpkt_subsys_cmd_code_type) UIMDIAG_MMGSDI_SESSION_GET_APP_CAPABILITIES_SYNC_CMD,
              rsp_id,
              sizeof(uimdiag_generic_rsp_type));

  if (delayed_rsp_ptr == NULL)
  {
    return NULL;
  }

  (void)memset((void*)&delayed_rsp_ptr->rsp_payload,0x00,sizeof(uimdiag_cmd_rsp_pyld_type));
   delayed_rsp_ptr->rsp_payload.mmgsdi_status = mmgsdi_status;

  if( mmgsdi_status == MMGSDI_SUCCESS )
  {
    delayed_rsp_ptr->rsp_payload.response.session_get_app_cap_sync_rsp.fdn_enabled =
      app_capabilities_sync.fdn_enabled;

    delayed_rsp_ptr->rsp_payload.response.session_get_app_cap_sync_rsp.bdn_enabled =
      app_capabilities_sync.bdn_enabled;

    delayed_rsp_ptr->rsp_payload.response.session_get_app_cap_sync_rsp.acl_enabled =
      app_capabilities_sync.acl_enabled;

    delayed_rsp_ptr->rsp_payload.response.session_get_app_cap_sync_rsp.imsi_invalidated =
      app_capabilities_sync.imsi_invalidated;

  }
  rex_sleep(10);
  diagpkt_delay_commit(delayed_rsp_ptr);
  return NULL;

}/* uimdiag_mmgsdi_session_get_app_capabilities_sync_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SAP_GET_ATR_CMD

DESCRIPTION
  Command handler to extract data for SAP GET ATR

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_sap_get_atr_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  int32                              index              = 0;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_client_id_type              client_id          = 0;
  mmgsdi_slot_id_enum_type           card_slot          = MMGSDI_MAX_SLOT_ID_ENUM;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_sap_get_atr_cmd");
  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  /* extract data from req packet
   and populate fields in the request structure */
  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SAP_GET_ATR_CMD);

  if( uimdiag_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_1("uimdiag_mmgsdi_parse_commands: Parsing error 0x%x",
                        uimdiag_status);
    return rsp_ptr;
  }
  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                                    UIMDIAG_MMGSDI_SAP_GET_ATR_CMD;

  UIMDIAG_TOUINT64(client_id,request.request.sap_get_atr.client_id_low,
                   request.request.sap_get_atr.client_id_high);

  card_slot     = (mmgsdi_slot_id_enum_type)request.request.sap_get_atr.card_slot;

  mmgsdi_status = mmgsdi_sap_get_atr(
                    client_id,
                    card_slot,
                    (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                    (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SAP_GET_ATR_CMD);

  if( delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG SAP GET ATR : Delayed Response error");
    return NULL;
  }

  return rsp_ptr;
}/* uimdiag_mmgsdi_sap_get_atr_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SAP_SEND_APDU_CMD

DESCRIPTION
  Command handler to extract data for SAP SEND APDU

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_sap_send_apdu_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  int32                              index              = 0;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_client_id_type              client_id          = 0;
  mmgsdi_slot_id_enum_type           card_slot          = MMGSDI_MAX_SLOT_ID_ENUM;
  int32                              channel_id         = 0;
  mmgsdi_sap_send_apdu_data_type     apdu_data;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_sap_send_apdu_cmd");

  (void)memset((void *)&request, 0x00, sizeof(uimdiag_generic_req_type));
  (void)memset((void *)&apdu_data, 0x00, sizeof(mmgsdi_sap_send_apdu_data_type));

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  /* extract data from req packet
   and populate fields in the request structure */
  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SAP_SEND_APDU_CMD);

  if( uimdiag_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_1("uimdiag_mmgsdi_parse_commands: Parsing error 0x%x",
                        uimdiag_status);
    return rsp_ptr;
  }
  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                                    UIMDIAG_MMGSDI_SAP_SEND_APDU_CMD;

  UIMDIAG_TOUINT64(client_id,request.request.sap_get_atr.client_id_low,
                   request.request.sap_get_atr.client_id_high);

  card_slot           = (mmgsdi_slot_id_enum_type)request.request.sap_send_apdu.card_slot;
  channel_id          = request.request.sap_send_apdu.channel_id;
  apdu_data.data_len  = request.request.sap_send_apdu.apdu_data_len;
  apdu_data.data_ptr  = request.request.sap_send_apdu.apdu_data_ptr;

  mmgsdi_status = mmgsdi_sap_send_apdu(
                    client_id,
                    card_slot,
                    channel_id,
                    apdu_data,
                    (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                    (mmgsdi_client_data_type)index);
  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SAP_SEND_APDU_CMD);

  if( delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG SAP SEND APDU : Delayed Response error");
    return NULL;
  }

  return rsp_ptr;
}/* uimdiag_mmgsdi_sap_send_apdu_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SAP_POWER_ON_CMD

DESCRIPTION
  Command handler to extract data for SAP POWER ON

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_sap_power_on_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  int32                              index              = 0;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_client_id_type              client_id          = 0;
  mmgsdi_slot_id_enum_type           card_slot          = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_sap_mode_enum_type          sap_mode           = MMGSDIBT_MAX_SAP_MODE_ENUM;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_sap_power_on_cmd");
  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  /* extract data from req packet
   and populate fields in the request structure */
  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SAP_POWER_ON_CMD);

  if( uimdiag_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_1("uimdiag_mmgsdi_parse_commands: Parsing error 0x%x",
                        uimdiag_status);
    return rsp_ptr;
  }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                                    UIMDIAG_MMGSDI_SAP_POWER_ON_CMD;

  UIMDIAG_TOUINT64(client_id,request.request.sap_power_on.client_id_low,
                   request.request.sap_power_on.client_id_high);

  card_slot     = (mmgsdi_slot_id_enum_type) request.request.sap_power_on.card_slot;
  sap_mode      = (mmgsdi_sap_mode_enum_type)request.request.sap_power_on.sap_mode;

  mmgsdi_status = mmgsdi_sap_power_on(
                    client_id,
                    card_slot,
                    sap_mode,
                    (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                    (mmgsdi_client_data_type)index);


  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SAP_POWER_ON_CMD);

  if( delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG SAP POWER ON : Delayed Response error");
    return NULL;
  }

  return rsp_ptr;
}/* uimdiag_mmgsdi_sap_power_on_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SAP_POWER_OFF_CMD

DESCRIPTION
  Command handler to extract data for SAP POWER OFF

DEPENDENCIES
  None.
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_sap_power_off_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  int32                              index              = 0;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_client_id_type              client_id          = 0;
  mmgsdi_slot_id_enum_type           card_slot          = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_sap_mode_enum_type          sap_mode           = MMGSDIBT_MAX_SAP_MODE_ENUM;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_sap_power_off_cmd");
  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  /* extract data from req packet
   and populate fields in the request structure */
  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SAP_POWER_OFF_CMD);

  if( uimdiag_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_1("uimdiag_mmgsdi_parse_commands: Parsing error 0x%x",
                        uimdiag_status);
    return rsp_ptr;
  }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                                    UIMDIAG_MMGSDI_SAP_POWER_OFF_CMD;

  UIMDIAG_TOUINT64(client_id,request.request.sap_power_off.client_id_low,
                   request.request.sap_power_off.client_id_high);

  card_slot     = (mmgsdi_slot_id_enum_type) request.request.sap_power_off.card_slot;
  sap_mode      = (mmgsdi_sap_mode_enum_type)request.request.sap_power_off.sap_mode;

  mmgsdi_status = mmgsdi_sap_power_off(
                    client_id,
                    card_slot,
                    sap_mode,
                    (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                    (mmgsdi_client_data_type)index);


  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SAP_POWER_OFF_CMD);

  if( delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG SAP POWER OFF : Delayed Response error");
    return NULL;
  }

  return rsp_ptr;
}/* uimdiag_mmgsdi_sap_power_off_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SAP_POWER_CARD_READER_STATUS_CMD

DESCRIPTION
  Command handler to extract data for SAP CARD READER STATUS

DEPENDENCIES
  None.
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_sap_card_reader_status_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  int32                              index              = 0;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_client_id_type              client_id          = 0;
  mmgsdi_slot_id_enum_type           card_slot          = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_sap_mode_enum_type          sap_mode           = MMGSDIBT_MAX_SAP_MODE_ENUM;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_sap_card_reader_status_cmd");
  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  /* extract data from req packet
   and populate fields in the request structure */
  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SAP_CARD_READER_STATUS_CMD);

  if( uimdiag_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_1("uimdiag_mmgsdi_parse_commands: Parsing error 0x%x",
                        uimdiag_status);
    return rsp_ptr;
  }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                                    UIMDIAG_MMGSDI_SAP_CARD_READER_STATUS_CMD;

  UIMDIAG_TOUINT64(client_id,request.request.sap_card_reader_status.client_id_low,
                   request.request.sap_card_reader_status.client_id_high);

  card_slot     = (mmgsdi_slot_id_enum_type) request.request.sap_card_reader_status.card_slot;
  sap_mode      = (mmgsdi_sap_mode_enum_type)request.request.sap_card_reader_status.sap_mode;

  mmgsdi_status = mmgsdi_sap_card_reader_status(
                    client_id,
                    card_slot,
                    sap_mode,
                    (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                    (mmgsdi_client_data_type)index);


  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SAP_CARD_READER_STATUS_CMD);

  if( delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG SAP CARD READER STATUS : Delayed Response error");
    return NULL;
  }

  return rsp_ptr;
}/* uimdiag_mmgsdi_sap_card_reader_status_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SAP_RESET_CMD

DESCRIPTION
  Command handler to extract data for SAP RESET

DEPENDENCIES
  None.
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_sap_reset_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  int32                              index              = 0;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_client_id_type              client_id          = 0;
  mmgsdi_slot_id_enum_type           card_slot          = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_sap_mode_enum_type          sap_mode           = MMGSDIBT_MAX_SAP_MODE_ENUM;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_sap_reset_cmd");
  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  /* extract data from req packet
   and populate fields in the request structure */
  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SAP_RESET_CMD);

  if( uimdiag_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_1("uimdiag_mmgsdi_parse_commands: Parsing error 0x%x",
                        uimdiag_status);
    return rsp_ptr;
  }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                                    UIMDIAG_MMGSDI_SAP_RESET_CMD;

  UIMDIAG_TOUINT64(client_id,request.request.sap_reset.client_id_low,
                   request.request.sap_reset.client_id_high);

  card_slot     = (mmgsdi_slot_id_enum_type) request.request.sap_reset.card_slot;
  sap_mode      = (mmgsdi_sap_mode_enum_type)request.request.sap_reset.sap_mode;

  mmgsdi_status = mmgsdi_sap_reset(
                    client_id,
                    card_slot,
                    sap_mode,
                    (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                    (mmgsdi_client_data_type)index);


  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SAP_RESET_CMD);

  if( delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG SAP RESET : Delayed Response error");
    return NULL;
  }

  return rsp_ptr;
}/* uimdiag_mmgsdi_sap_reset_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_CARD_PDOWN_CMD

DESCRIPTION
  Command handler to extract data for CARD PDOWN

DEPENDENCIES
  None.
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_card_pdown_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type            request;
  diagpkt_subsys_delayed_rsp_id_type  rsp_id             = 0;
  int32                               index              = 0;
  uimdiag_generic_rsp_type          * rsp_ptr            = NULL;
  uimdiag_return_enum_type            uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type            delayed_rsp_status = UIMDIAG_ERROR;
  mmgsdi_return_enum_type             mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_client_id_type               client_id          = 0;
  mmgsdi_slot_id_enum_type            slot_id            = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_card_pdown_options_enum_type pdown_option       = MMGSDI_CARD_PDOWN_MAX_ENUM;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_card_pdown_cmd");
  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  /* extract data from req packet
   and populate fields in the request structure */
  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_CARD_PDOWN_CMD);

  if( uimdiag_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_1("uimdiag_mmgsdi_parse_commands: Parsing error 0x%x",
                        uimdiag_status);
    return rsp_ptr;
  }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                                    UIMDIAG_MMGSDI_CARD_PDOWN_CMD;

  UIMDIAG_TOUINT64(client_id,request.request.card_pdown.client_id_low,
                   request.request.card_pdown.client_id_high);

  slot_id = (mmgsdi_slot_id_enum_type)request.request.card_pdown.slot_id;
  pdown_option = (mmgsdi_card_pdown_options_enum_type)request.request.card_pdown.power_down_option;

  mmgsdi_status = mmgsdi_card_pdown(
                    client_id,
                    slot_id,
                    (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                    pdown_option,
                    (mmgsdi_client_data_type)index);


  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_CARD_PDOWN_CMD);

  if( delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG CARD PDOWN : Delayed Response error");
    return NULL;
  }

  return rsp_ptr;
}/* uimdiag_mmgsdi_card_pdown_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_CARD_PUP_CMD

DESCRIPTION
  Command handler to extract data for CARD PUP

DEPENDENCIES
  None.
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_card_pup_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type            request;
  diagpkt_subsys_delayed_rsp_id_type  rsp_id             = 0;
  int32                               index              = 0;
  uimdiag_generic_rsp_type          * rsp_ptr            = NULL;
  uimdiag_return_enum_type            uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type            delayed_rsp_status = UIMDIAG_ERROR;
  mmgsdi_return_enum_type             mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_client_id_type               client_id          = 0;
  mmgsdi_slot_id_enum_type            slot_id            = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_card_pup_options_enum_type   pup_option         = MMGSDI_CARD_PUP_MAX_ENUM;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_card_pup_cmd");
  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  /* extract data from req packet
   and populate fields in the request structure */
  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_CARD_PUP_CMD);

  if( uimdiag_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_1("uimdiag_mmgsdi_parse_commands: Parsing error 0x%x",
                        uimdiag_status);
    return rsp_ptr;
  }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                                    UIMDIAG_MMGSDI_CARD_PUP_CMD;

  UIMDIAG_TOUINT64(client_id,request.request.card_pup.client_id_low,
                   request.request.card_pup.client_id_high);

  slot_id = (mmgsdi_slot_id_enum_type)request.request.card_pup.slot_id;
  pup_option = (mmgsdi_card_pup_options_enum_type)request.request.card_pup.power_up_option;

  mmgsdi_status = mmgsdi_card_pup(
                    client_id,
                    slot_id,
                    (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                    pup_option,
                    (mmgsdi_client_data_type)index);


  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_CARD_PUP_CMD);

  if( delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG CARD PUP : Delayed Response error");
    return NULL;
  }

  return rsp_ptr;
}/* uimdiag_mmgsdi_card_pup_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_ACTIVATE_ONCHIP_CMD

DESCRIPTION
  Command handler to extract data for activating ONCHIP

DEPENDENCIES
  None.
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static PACKED void * uimdiag_mmgsdi_activate_onchip_cmd (
  PACKED void                     * cmd_ptr,
  word                              pkt_len,
  uimdiag_subsystem_cmd_enum_type   cmd_enum
)
{
  uimdiag_generic_req_type            request;
  diagpkt_subsys_delayed_rsp_id_type  rsp_id             = 0;
  int32                               index              = 0;
  uimdiag_generic_rsp_type          * rsp_ptr            = NULL;
  uimdiag_return_enum_type            uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type            delayed_rsp_status = UIMDIAG_ERROR;
  mmgsdi_return_enum_type             mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_client_id_type               client_id          = 0;
  mmgsdi_slot_id_enum_type            slot_id            = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_onchip_sim_config_data_type  onchip_sim_config;

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );
  (void)memset((void*)&onchip_sim_config, 0x00, sizeof(mmgsdi_onchip_sim_config_data_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  if(cmd_enum != UIMDIAG_MMGSDI_ACTIVATE_ONCHIP_SIM_CMD &&
     cmd_enum != UIMDIAG_MMGSDI_ACTIVATE_ONCHIP_USIM_CMD)
  {
    UIMDIAG_MSG_ERROR_1("invalid onchip command: cmd_enum = 0x%x", cmd_enum);
    return NULL;
  }

  /* extract data from req packet
   and populate fields in the request structure */
  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     cmd_enum);

  if( uimdiag_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_1("uimdiag_mmgsdi_parse_commands: Parsing error 0x%x",
                        uimdiag_status);
    return rsp_ptr;
  }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd = cmd_enum;

  UIMDIAG_TOUINT64(client_id,request.request.activate_onchip_sim.client_id_low,
                   request.request.activate_onchip_sim.client_id_high);

  slot_id = (mmgsdi_slot_id_enum_type)request.request.activate_onchip_sim.slot_id;

  onchip_sim_config.data_len = request.request.activate_onchip_sim.onchip_data_length;

  if(request.request.activate_onchip_sim.onchip_data_length != 0)
  {
    onchip_sim_config.data_ptr = request.request.activate_onchip_sim.onchip_data_ptr;
  }
  else
  {
    UIMDIAG_MSG_ERROR_0("request.request.activate_onchip_sim.onchip_data_len == 0");
  }

  if(cmd_enum == UIMDIAG_MMGSDI_ACTIVATE_ONCHIP_SIM_CMD)
  {
    mmgsdi_status = mmgsdi_activate_onchip_sim(
                      client_id,
                      slot_id,
                      onchip_sim_config,
                      (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                      (mmgsdi_client_data_type)index);
  }
  else
  {
    mmgsdi_status = mmgsdi_activate_onchip_usim(
                      client_id,
                      slot_id,
                      onchip_sim_config,
                      (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                      (mmgsdi_client_data_type)index);
  }

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    cmd_enum);

  if( delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG ACTIVATE ONCHIP : Delayed Response error");
    return NULL;
  }

  UIMDIAGUTIL_TMC_MEM_FREE(request.request.activate_onchip_sim.onchip_data_ptr);
  return rsp_ptr;
}/* uimdiag_mmgsdi_activate_onchip_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_ACTIVATE_ONCHIP_SIM_CMD

DESCRIPTION
  Command handler to extract data for ACTIVATE ONCHIP SIM

DEPENDENCIES
  None.
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_activate_onchip_sim_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_activate_onchip_sim_cmd");

  return uimdiag_mmgsdi_activate_onchip_cmd(
           cmd_ptr,
           pkt_len,
           UIMDIAG_MMGSDI_ACTIVATE_ONCHIP_SIM_CMD);
}/* uimdiag_mmgsdi_activate_onchip_sim_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_ACTIVATE_ONCHIP_USIM_CMD

DESCRIPTION
  Command handler to extract data for ACTIVATE ONCHIP USIM

DEPENDENCIES
  None.
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_activate_onchip_usim_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_activate_onchip_usim_cmd");

  return uimdiag_mmgsdi_activate_onchip_cmd(
           cmd_ptr,
           pkt_len,
           UIMDIAG_MMGSDI_ACTIVATE_ONCHIP_USIM_CMD);
}/* uimdiag_mmgsdi_activate_onchip_usim_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SEND_APDU_CMD

DESCRIPTION
  Command handler to send apdus

DEPENDENCIES
  None.
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_send_apdu_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type            request;
  diagpkt_subsys_delayed_rsp_id_type  rsp_id             = 0;
  int32                               index              = 0;
  uimdiag_generic_rsp_type          * rsp_ptr            = NULL;
  uimdiag_return_enum_type            uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type            delayed_rsp_status = UIMDIAG_ERROR;
  mmgsdi_return_enum_type             mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_client_id_type               client_id          = 0;
  mmgsdi_slot_id_enum_type            slot_id            = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_send_apdu_data_type          apdu_data;
  mmgsdi_option_type                  option;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_send_apdu_cmd");

  (void)memset((void *)&request, 0x00, sizeof(uimdiag_generic_req_type));
  (void)memset((void *)&apdu_data, 0x00, sizeof(mmgsdi_send_apdu_data_type));
  (void)memset((void *)&option, 0x00, sizeof(mmgsdi_option_type));

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  /* extract data from req packet
   and populate fields in the request structure */
  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SEND_APDU_CMD);

  if( uimdiag_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_1("uimdiag_mmgsdi_parse_commands: Parsing error 0x%x",
                        uimdiag_status);
    return rsp_ptr;
  }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                                    UIMDIAG_MMGSDI_SEND_APDU_CMD;

  slot_id = (mmgsdi_slot_id_enum_type)request.request.send_apdu.slot_id;

  UIMDIAG_TOUINT64(client_id,request.request.send_apdu.client_id_low,
                   request.request.send_apdu.client_id_high);

  apdu_data.data_len = request.request.send_apdu.apdu_data_length;

  if(request.request.send_apdu.apdu_data_length != 0)
  {
    apdu_data.data_ptr = request.request.send_apdu.apdu_data_ptr;
  }
  else
  {
    UIMDIAG_MSG_ERROR_0("request.request.send_apdu.send_apdu_length == 0");
  }

  mmgsdi_status = mmgsdi_send_apdu(
                    client_id,
                    slot_id,
                    apdu_data,
                    option,
                    (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                    (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SEND_APDU_CMD);

  if( delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG SEND APDU SIM : Delayed Response error");
    return NULL;
  }

  UIMDIAGUTIL_TMC_MEM_FREE(request.request.send_apdu.apdu_data_ptr);
  return rsp_ptr;
}/* uimdiag_mmgsdi_send_apdu_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SAP_CONNECT

DESCRIPTION
  Command handler to extract data for SAP CONNECT

DEPENDENCIES
  None.
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_sap_connect_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  int32                              index              = 0;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_client_id_type              client_id          = 0;
  mmgsdi_slot_id_enum_type           card_slot          = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_sap_mode_enum_type          sap_mode           = MMGSDIBT_MAX_SAP_MODE_ENUM;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_sap_connect_cmd");
  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  /* extract data from req packet
   and populate fields in the request structure */
  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SAP_CONNECT_CMD);

  if( uimdiag_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_1("uimdiag_mmgsdi_parse_commands: Parsing error 0x%x",
                        uimdiag_status);
    return rsp_ptr;
  }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd = UIMDIAG_MMGSDI_SAP_CONNECT_CMD;

  UIMDIAG_TOUINT64(client_id,request.request.sap_connect.client_id_low,
                   request.request.sap_connect.client_id_high);

  card_slot     = (mmgsdi_slot_id_enum_type)request.request.sap_connect.card_slot;
  sap_mode      = (mmgsdi_sap_mode_enum_type)request.request.sap_connect.sap_mode;

  mmgsdi_status = mmgsdi_sap_connect(
                    client_id,
                    card_slot,
                    sap_mode,
                    (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                    (mmgsdi_client_data_type)index);


  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SAP_CONNECT_CMD);

  if( delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG SAP DEREGISTER : Delayed Response error");
    return NULL;
  }

  return rsp_ptr;
}/* uimdiag_mmgsdi_sap_connect_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SAP_DISCONNECT_CMD

DESCRIPTION
  Command handler to extract data for SAP DISCONNECT

DEPENDENCIES
  None.
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_sap_disconnect_cmd(
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  int32                              index              = 0;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_client_id_type              client_id          = 0;
  mmgsdi_slot_id_enum_type           card_slot          = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_sap_mode_enum_type          sap_mode           = MMGSDIBT_MAX_SAP_MODE_ENUM;
  mmgsdi_disconnect_mode_enum_type   disconnect_mode    = MMGSDIBT_MAX_DISCONNECT_MODE_ENUM;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_sap_disconnect_cmd");
  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  /* extract data from req packet
   and populate fields in the request structure */
  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SAP_DISCONNECT_CMD);

  if( uimdiag_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_1("uimdiag_mmgsdi_parse_commands: Parsing error 0x%x",
                        uimdiag_status);
    return rsp_ptr;
  }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd = UIMDIAG_MMGSDI_SAP_DISCONNECT_CMD;

  UIMDIAG_TOUINT64(client_id,request.request.sap_disconnect.client_id_low,
                   request.request.sap_disconnect.client_id_high);

  card_slot       = (mmgsdi_slot_id_enum_type)request.request.sap_disconnect.card_slot;
  sap_mode        = (mmgsdi_sap_mode_enum_type)request.request.sap_disconnect.sap_mode;
  disconnect_mode = (mmgsdi_disconnect_mode_enum_type)request.request.sap_disconnect.sap_disconnect_mode;

  mmgsdi_status = mmgsdi_sap_disconnect(
                    client_id,
                    card_slot,
                    sap_mode,
                    disconnect_mode,
                    (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                    (mmgsdi_client_data_type)index);


  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SAP_DISCONNECT_CMD);

  if( delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG SAP DEREGISTER : Delayed Response error");
    return NULL;
  }

  return rsp_ptr;
}/* uimdiag_mmgsdi_sap_disconnect_cmd */
/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_GET_OPERATOR_NAME_TABLE_CMD

DESCRIPTION
  Command handler for getting operator Name(s)

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_get_operator_name_table_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  int32                              index              = 0;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_return_enum_type           delayed_rsp_status = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_session_id_type             session_id;
  mmgsdi_plmn_id_list_type           plmn_id_list;
  uint32                             loop_cnt           = 0;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_get_operator_name_table_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_SESSION_GET_OPERATOR_NAME_CMD);

  if(uimdiag_status != UIMDIAG_SUCCESS)
  {
    return rsp_ptr;
  }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                                    UIMDIAG_MMGSDI_SESSION_GET_OPERATOR_NAME_CMD;

  session_id         = request.request.session_get_operator_name.session_id_low;
  plmn_id_list.num_of_plmn_ids = request.request.session_get_operator_name.no_of_plmns;
  plmn_id_list.plmn_list_ptr   = NULL;

  if(plmn_id_list.num_of_plmn_ids > 0)
  {
    /* Do memory allocation for plmn_id_list.plmn_list_ptr and copy plmn id
       and RAT type from plmn_list_ptr (buffer pointer). plmn_list_ptr can not
       be typecasted to mmgsdi_plmn_id_type pointer type */
    UIMDIAG_UTIL_TMC_MEM_MALLOC_AND_VALIDATE(
        plmn_id_list.plmn_list_ptr,
        (plmn_id_list.num_of_plmn_ids * sizeof(mmgsdi_plmn_id_type)));

    if(plmn_id_list.plmn_list_ptr == NULL)
    {
      return NULL;
    }

    for(loop_cnt = 0; loop_cnt < plmn_id_list.num_of_plmn_ids; loop_cnt++)
    {
      memscpy(plmn_id_list.plmn_list_ptr[loop_cnt].plmn_id_val,
             sizeof(plmn_id_list.plmn_list_ptr[loop_cnt].plmn_id_val),
             &request.request.session_get_operator_name.plmn_list_ptr[loop_cnt * 4],
             MMGSDI_PLMN_ID_SIZE);

      plmn_id_list.plmn_list_ptr[loop_cnt].rat =
        (mmgsdi_rat_enum_type)request.request.session_get_operator_name.plmn_list_ptr[(loop_cnt * 4) + MMGSDI_PLMN_ID_SIZE];
    }
  }

  mmgsdi_status = mmgsdi_session_get_operator_name(
                    session_id,
                    plmn_id_list,
                    (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                    (mmgsdi_client_data_type)index);

  UIMDIAGUTIL_TMC_MEM_FREE(plmn_id_list.plmn_list_ptr);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SESSION_GET_OPERATOR_NAME_CMD);

  if( delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG_MMGSDI_SESSION_GET_OPERATOR_NAME_CMD: Delayed Response error");
    return NULL;
  }

  return rsp_ptr;
}/* uimdiag_mmgsdi_session_get_operator_name_table_cmd */


/*===========================================================================

FUNCTION uimdiag_mmgsdi_get_all_available_apps_sync_cmd

DESCRIPTION
  Command handler to get all available apps.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_get_all_available_apps_sync_cmd (
  PACKED void *cmd_ptr,
  word        pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  uimdiag_generic_rsp_type          *delayed_rsp_ptr    = NULL;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_client_id_type              client_id;
  mmgsdi_available_apps_type        *available_apps_ptr = NULL;
  uint32                             len                = 0;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_get_all_available_apps_sync_cmd");
  memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if( cmd_ptr == NULL)
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL");
    return NULL;
  }

  /* extract data from req packet
   and populate fields in the request structure */
  uimdiag_status = uimdiag_parse_cmd_data(&request, cmd_ptr, pkt_len);

  if( uimdiag_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_1("uimdiag_mmgsdi_parse_commands: Parsing error 0x%x",
                        uimdiag_status);
    return NULL;
  }

  UIMDIAG_UTIL_TMC_MEM_MALLOC_AND_VALIDATE(
    available_apps_ptr,
    sizeof(mmgsdi_available_apps_type));

  if(available_apps_ptr == NULL)
  {
    return NULL;
  }

  /*  allocate immediate response pointer  */
  rsp_ptr = (uimdiag_generic_rsp_type *)
                diagpkt_subsys_alloc_v2(
                  (diagpkt_subsys_id_type) DIAG_SUBSYS_GSDI,
                  (diagpkt_subsys_cmd_code_type) UIMDIAG_MMGSDI_GET_ALL_AVAIL_APP_SYNC_CMD,
                  sizeof(uimdiag_generic_rsp_type));

  if( rsp_ptr == NULL)
  {
    UIMDIAG_MSG_ERROR_0("rsp_ptr == NULL");
    UIMDIAGUTIL_TMC_MEM_FREE(available_apps_ptr);
    return NULL;
  }

  rsp_id = diagpkt_subsys_get_delayed_rsp_id(rsp_ptr);

  UIMDIAG_TOUINT64(client_id,
                   request.request.get_all_available_apps.client_id_low,
                   request.request.get_all_available_apps.client_id_high);

  mmgsdi_status = mmgsdi_get_all_available_apps_sync(client_id, available_apps_ptr);


  diagpkt_subsys_set_status(rsp_ptr, (uint32) UIMDIAG_SUCCESS);
  diagpkt_commit(rsp_ptr);

  if( mmgsdi_status == MMGSDI_SUCCESS )
  {
    /*  Determine the len of the response pointer  */

    /* for header, 4 bytes for status and one set app info by default */
    len   = sizeof(uimdiag_cmd_rsp_hdr_type) +
            sizeof(uimdiag_mmgsdi_get_all_available_apps_rsp_type) + 4;

    /* len for one app is already included in
       uimdiag_mmgsdi_get_all_available_apps_rsp_type */
    if((available_apps_ptr->card1_num_app + available_apps_ptr->card2_num_app)  > 0)
    {
      len += ((available_apps_ptr->card1_num_app + available_apps_ptr->card2_num_app) -1) *
               sizeof(uimdiag_mmgsdi_app_info_type);
    }
  }
  else
  {
    /* since command handling is not successful - allocate a default buffer to hold status */
    len =  sizeof(uimdiag_generic_rsp_type);
  }

    /*  allocate delayed response pointer  */
  delayed_rsp_ptr = (uimdiag_generic_rsp_type *)
                diagpkt_subsys_alloc_v2_delay(
                  (diagpkt_subsys_id_type) DIAG_SUBSYS_GSDI,
                  (diagpkt_subsys_cmd_code_type) UIMDIAG_MMGSDI_GET_ALL_AVAIL_APP_SYNC_CMD,
                  rsp_id,
                  len);

  if( delayed_rsp_ptr == NULL)
  {
    UIMDIAG_MSG_ERROR_0("rsp_ptr == NULL");
    UIMDIAGUTIL_TMC_MEM_FREE(available_apps_ptr);
    return NULL;
  }

  (void)memset((void*)&delayed_rsp_ptr->rsp_payload,
               0x00,
               len - sizeof(uimdiag_cmd_rsp_hdr_type));

  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
    uimdiag_status =
        uimdiag_mmgsdi_get_all_available_apps_cnf(delayed_rsp_ptr,available_apps_ptr);
  }

  delayed_rsp_ptr->rsp_payload.mmgsdi_status = mmgsdi_status;

  rex_sleep(10);
  diagpkt_delay_commit(delayed_rsp_ptr);
  UIMDIAGUTIL_TMC_MEM_FREE(available_apps_ptr);
  return NULL;
}/* uimdiag_mmgsdi_get_all_available_apps_sync_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_IS_SERVICE_AVAILABLE_SYNC_CMD

DESCRIPTION
  Command handler for checking if a particular service is available. (Sync API)

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_is_service_available_sync_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr            = NULL;
  diagpkt_subsys_delayed_rsp_id_type rsp_id             = 0;
  uimdiag_return_enum_type           uimdiag_status     = UIMDIAG_ERROR;
  uimdiag_generic_rsp_type          *delayed_rsp_ptr    = NULL;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_service_enum_type           service            = MMGSDI_NONE;
  boolean                            is_svc_available   = 0;
  mmgsdi_session_id_type             session_id;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_is_service_available_sync_cmd");

  (void)memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if (cmd_ptr == NULL)
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL");
    return NULL;
  }

/* extract data from req packet
   and populate fields in the request structure */
  uimdiag_status = uimdiag_parse_cmd_data(&request, cmd_ptr, pkt_len);

  if(uimdiag_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_1("uimdiag_mmgsdi_parse_commands: Parsing error 0x%x",
                        uimdiag_status);
    return NULL;
  }
  /*  allocate response pointer  */
  rsp_ptr = (uimdiag_generic_rsp_type *)
                diagpkt_subsys_alloc_v2(
                  (diagpkt_subsys_id_type) DIAG_SUBSYS_GSDI,
                  (diagpkt_subsys_cmd_code_type) UIMDIAG_MMGSDI_IS_SERVICE_AVAILABLE_SYNC_CMD,
                  sizeof(uimdiag_generic_rsp_type));

  if( rsp_ptr == NULL)
  {
    UIMDIAG_MSG_ERROR_0("rsp_ptr == NULL");
    return NULL;
  }
  rsp_id = diagpkt_subsys_get_delayed_rsp_id(rsp_ptr );


  session_id         = request.request.is_service_available_sync.session_id_low;
  service            = (mmgsdi_service_enum_type)
                         request.request.is_service_available_sync.service;

  mmgsdi_status       = mmgsdi_session_is_service_available_sync(
                          session_id,
                          service,
                          &is_svc_available);

  diagpkt_subsys_set_status(rsp_ptr, (uint32) UIMDIAG_SUCCESS);
  diagpkt_commit(rsp_ptr);

  /* allocate delayed response pointer */
  delayed_rsp_ptr = (uimdiag_generic_rsp_type *)
           diagpkt_subsys_alloc_v2_delay(
             (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
             (diagpkt_subsys_cmd_code_type) UIMDIAG_MMGSDI_IS_SERVICE_AVAILABLE_SYNC_CMD,
             rsp_id,
             sizeof(uimdiag_generic_rsp_type));

  if (delayed_rsp_ptr == NULL)
  {
    UIMDIAG_MSG_ERROR_0("delayed_rsp_ptr == NULL");
    return NULL;
  }

  (void)memset((void*)&delayed_rsp_ptr->rsp_payload,0x00,sizeof(uimdiag_cmd_rsp_pyld_type));

  delayed_rsp_ptr->rsp_payload.mmgsdi_status = mmgsdi_status;

  if( mmgsdi_status == MMGSDI_SUCCESS )
  {
    delayed_rsp_ptr->rsp_payload.response.is_service_available_sync_rsp.service_type =
      (uint32)is_svc_available;
  }
  rex_sleep(10);
  diagpkt_delay_commit(delayed_rsp_ptr);
  return NULL;

}/* uimdiag_mmgsdi_is_service_available_sync_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_OPEN_EXT_CMD

DESCRIPTION
  Command handler for multiple session open subsystem command

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_open_ext_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{

  uimdiag_generic_req_type              request;
  uimdiag_generic_rsp_type             *rsp_ptr                = NULL;
  uimdiag_return_enum_type              delayed_rsp_status     = UIMDIAG_ERROR;
  diagpkt_subsys_delayed_rsp_id_type    rsp_id                 = 0;
  uimdiag_return_enum_type              uimdiag_status         = UIMDIAG_ERROR;
  uimdiag_mmgsdi_session_open_ext_cmd_type *session_open_req_ptr   = NULL;
  mmgsdi_client_id_type                 client_id              = 0;
  int32                                 index                  = 0;
  mmgsdi_return_enum_type               mmgsdi_status          = MMGSDI_ERROR;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_open_ext_cmd");

  (void)memset ( (void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr, pkt_len, &rsp_ptr, &rsp_id, &index,
                     &request, UIMDIAG_MMGSDI_SESSION_OPEN_EXT_CMD);

  if((uimdiag_status != UIMDIAG_SUCCESS))
  {
    return rsp_ptr;
  }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd = UIMDIAG_MMGSDI_SESSION_OPEN_EXT_CMD;

  session_open_req_ptr = &request.request.session_open_ext;
  UIMDIAG_TOUINT64(client_id, session_open_req_ptr->client_id_low,
                   session_open_req_ptr->client_id_high);


   mmgsdi_status = mmgsdi_session_open_ext (
                    client_id,
                    (uint32)
                    session_open_req_ptr->session_type_mask,
                    uimdiag_mmgsdi_event_cb,
                    (uint32)session_open_req_ptr->set_notify_mask,
                    (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                    (mmgsdi_client_data_type)index);


  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_SESSION_OPEN_EXT_CMD);

  if( delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG Session Open Ext: Delayed Response error");
    return NULL;
  }

  return rsp_ptr;
}/* uimdiag_mmgsdi_session_open_ext_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_GET_NVRUIM_DATA_SYNC_CMD

DESCRIPTION
This function allows a client to query MMGSDI to get nvruim state information and part of
nvruim's global cache. This is a synchronous call.


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_get_nvruim_data_sync_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  uimdiag_generic_req_type           request;
  uimdiag_generic_rsp_type          *rsp_ptr           = NULL;
  mmgsdi_session_id_type             session_id;
  diagpkt_subsys_delayed_rsp_id_type rsp_id            = 0;
  uimdiag_generic_rsp_type          *delayed_rsp_ptr   = NULL;
  uimdiag_return_enum_type           uimdiag_status    = UIMDIAG_ERROR;
  mmgsdi_return_enum_type            mmgsdi_status     = MMGSDI_ERROR;
  mmgsdi_nvruim_data_type            nvruim_data;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_session_get_nvruim_data_sync_cmd");
  memset((void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if( cmd_ptr == NULL)
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL");
    return NULL;
  }

  /* extract data from req packet
   and populate fields in the request structure */
  uimdiag_status = uimdiag_parse_cmd_data(&request, cmd_ptr, pkt_len);

  if( uimdiag_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_1("uimdiag_mmgsdi_parse_commands: Parsing error 0x%x",
                        uimdiag_status);
    return NULL;
  }

  /*  allocate response pointer  */
  rsp_ptr = (uimdiag_generic_rsp_type *)
                diagpkt_subsys_alloc_v2(
                  (diagpkt_subsys_id_type) DIAG_SUBSYS_GSDI,
                  (diagpkt_subsys_cmd_code_type)
                  UIMDIAG_MMGSDI_SESSION_GET_NVRUIM_DATA_SYNC_CMD,
                  sizeof(uimdiag_generic_rsp_type));

  if (rsp_ptr == NULL)
  {
    UIMDIAG_MSG_ERROR_0("rsp_ptr == NULL");
    return rsp_ptr;
  }

  rsp_id = diagpkt_subsys_get_delayed_rsp_id(rsp_ptr );

  session_id         =
    request.request.session_get_nvruim_data_sync.session_id_low;

  mmgsdi_status = mmgsdi_session_get_nvruim_data_sync(
                    session_id,
                    &nvruim_data);

  UIMDIAG_MSG_HIGH_1(" nvruim mmgsdi call MMGSDI Status is %d ",
                     mmgsdi_status);

  diagpkt_subsys_set_status(rsp_ptr, (uint32) UIMDIAG_SUCCESS);
  diagpkt_commit(rsp_ptr);
  UIMDIAG_MSG_HIGH_0(" Immediate commit in nvruim data sync ");

  /* allocate delayed response pointer */
  delayed_rsp_ptr = (uimdiag_generic_rsp_type *)
           diagpkt_subsys_alloc_v2_delay(
             (diagpkt_subsys_id_type) DIAG_SUBSYS_GSDI,
             (diagpkt_subsys_cmd_code_type)
             UIMDIAG_MMGSDI_SESSION_GET_NVRUIM_DATA_SYNC_CMD,
             rsp_id,
             sizeof(uimdiag_generic_rsp_type));

  if (delayed_rsp_ptr == NULL)
  {
    UIMDIAG_MSG_ERROR_0("delayed_rsp_ptr == NULL");
    return NULL;
  }

  (void)memset((void*)&delayed_rsp_ptr->rsp_payload,0x00,
                      sizeof(uimdiag_cmd_rsp_pyld_type));

  delayed_rsp_ptr->rsp_payload.mmgsdi_status = mmgsdi_status;

  if( mmgsdi_status == MMGSDI_SUCCESS )
  {
    uimdiag_status =
      uimdiag_mmgsdi_session_get_nvruim_data_sync_cnf(delayed_rsp_ptr,&nvruim_data);
  }
  rex_sleep(10);
  diagpkt_delay_commit(delayed_rsp_ptr);
  return NULL;
}/* uimdiag_mmgsdi_session_get_nvruim_data_sync_cmd */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_get_all_available_apps_async_cmd

DESCRIPTION
  Command handler for get all available apps async

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_get_all_available_apps_async_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{

  uimdiag_generic_req_type              request;
  uimdiag_generic_rsp_type             *rsp_ptr                = NULL;
  uimdiag_return_enum_type              delayed_rsp_status     = UIMDIAG_ERROR;
  diagpkt_subsys_delayed_rsp_id_type    rsp_id                 = 0;
  uimdiag_return_enum_type              uimdiag_status         = UIMDIAG_ERROR;
  mmgsdi_client_id_type                 client_id              = 0;
  int32                                 index                  = 0;
  mmgsdi_return_enum_type               mmgsdi_status          = MMGSDI_ERROR;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_get_all_available_apps_async_cmd");

  (void)memset ( (void*)&request, 0x00, sizeof(uimdiag_generic_req_type) );

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr, pkt_len, &rsp_ptr, &rsp_id, &index,
                     &request, UIMDIAG_MMGSDI_GET_ALL_AVAILABLE_APPS_CMD);

  if((uimdiag_status != UIMDIAG_SUCCESS))
  {
    return rsp_ptr;
  }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd = UIMDIAG_MMGSDI_GET_ALL_AVAILABLE_APPS_CMD;


  UIMDIAG_TOUINT64(client_id,request.request.get_all_available_apps.client_id_low,
                   request.request.get_all_available_apps.client_id_high);
   mmgsdi_status      = mmgsdi_get_all_available_apps(client_id,
                                           (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                                           (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_GET_ALL_AVAILABLE_APPS_CMD);

  if( delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG Session Open Ext: Delayed Response error");
    return NULL;
  }

  return rsp_ptr;
}/* uimdiag_mmgsdi_get_all_available_apps_async_cmd */
#endif /* FEATURE_UIMDIAG */
