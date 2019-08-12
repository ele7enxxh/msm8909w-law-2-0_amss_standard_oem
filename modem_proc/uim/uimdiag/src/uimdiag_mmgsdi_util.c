/*===========================================================================

===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2009 - 2014, 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdiag/src/uimdiag_mmgsdi_util.c#1 $$ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who   what, where, why
--------   ---   -----------------------------------------------------------
05/22/16   vdc   Remove F3 messages for memory allocation failure
01/27/14   df    Featurise file with FEATURE_UIMDIAG
01/10/14   df    Use v2.0 diag macros
12/11/13   df    Remove unused memory paging feature
11/05/13   df    Remove unnecessary include tmc.h
11/05/13   df    Fix LLVM compiler issue in util functions
09/25/13   bcho  Query type parsed in UIMDIAG_MMGSDI_SESSION_GET_INFO_CMD
05/06/13   sw    Changes made for secure version of memcpy
04/24/13   rr    Featurized optional logs
11/23/12   bcho  Support for RAT type param in get_operator_name API
11/09/12   yt    Remove parsing of length field for PERSO_GET_DATA_CMD
09/25/12   tl    Adding nvruim_data_req_enum_type to sessionid_type
10/24/11   adp   Removing Num_of_files from request field for naa_refresh
10/24/11   adp   Removing support for UIMDIAG_MMGSDI_SAP_PIN_VERIFY_CMD,
                 UIMDIAG_MMGSDI_SAP_INITIALIZE_CMD and
                 UIMDIAG_MMGSDI_SAP_DEREGISTER_CMD
10/10/11   adp   Adding uimdiag enum mapping
10/18/11   bcho  Adding support for mmgsdi_session_open_ext API
10/14/11   bcho  Added support for Get Operator Name API
10/10/11   nb    Fixed Incorrect featurization due to merge error
09/23/11   nb    Support for SAP CONNECT/SAP DISCONNECT
09/15/11   adp   Adding support to trigger refresh in uimdiag
09/15/11   nb    Support for new refresh stage
08/22/11   av    Added support for pdown,pup,activate onchip,send apdu commands
08/05/11   av    UIMDiag now doesn't return error if search pattern is null;
                 It's handled in mmgsdi now.
06/09/11   adp   Fixing get file attr by path access, Fixing search and seek
04/06/11   adp   Swapping the fileds for set notify and exclusive channel
                 to match with the ICD and the test scripts.
02/02/10   ms    Fixed compiler warnings
12/29/10   ms    Adding support for BT SAP APIs
12/03/10   adp   Adding support for a number of commands
10/08/10   adp   Changing the offset to get the correct enum from
                 mmgsdilib_common.h
09/07/10   adp   Adding Support for dck_num_retries,lock_down_enable_ltd_access,
                 seek and search commands
08/09/10   adp   Adding support for service enable, disable, get perso key,
                 is service available, perso unblock.
04/07/10   adp   Removing check for cmd_ptr->path_len == 0 check.
                 Path len could be zero.
03/25/10   adp   Adding support for mmgsdi_session_deactivate_provisioning,
                 mmgsdi_session_get_app_capabilities,
                 mmgsdi_session_get_cphs_info,
                 isim_authenticate,send_card_status,
                 create_pkcs15_lookup_table
03/23/10   adp   Fixing check for ASSERT
03/19/10   rk    Added support for JCDMA get info
02/22/10   adp   Adding support for Perso Commands
12/27/09   ap    Adding support for Refresh commands
12/03/09   rk    Adding support for get file attributes, read and write commands
10/18/09   adp   Initial Revision -Added mmgsdi util functions


=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "uim_variation.h"
#ifdef FEATURE_UIMDIAG
#include "diagcmd.h"
#include "uimdiag.h"
#include "uimdiag_mmgsdi.h"
#include "uimdiag_mmgsdi_util.h"
#include "uim_msg.h"
#include "uimdiag_util.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
static const void* uimdiag_req_ptr;
static uint32 uimdiag_req_len;
static uint32 uimdiag_offset;
static uimdiag_return_enum_type uimdiag_decode_status;

/*=============================================================================

                              FUNCTIONS

=============================================================================*/

/*===========================================================================

FUNCTION uimdiag_mmgsdi_util_init_decode_params

DESCRIPTION
  This function should be called before any structure is decoded.

  This function copies diag_req_ptr,diag_req_len to static variables
  and initializes the offset and uimdiag_decode_status.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
static void uimdiag_mmgsdi_util_init_decode_params(
    const void                                         * diag_req_ptr,
    uint32                                               diag_req_len)
{
  uimdiag_req_ptr = diag_req_ptr;
  uimdiag_req_len = diag_req_len;
  uimdiag_decode_status = UIMDIAG_SUCCESS;
  uimdiag_offset = 0;
} /* uimdiag_mmgsdi_util_decode_uint8 */


/*===========================================================================

FUNCTION uimdiag_mmgsdi_util_decode_uint8

DESCRIPTION
  This function copies uint8 from diag_req_ptr to cmd_ptr,
  from offset value.
  Before copy, it checks diag_req_len, to validate the data in diag_req_ptr.
  The offset is updated after the copy.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/

static void uimdiag_mmgsdi_util_decode_uint8(void *cmd_ptr)
{
  if (uimdiag_decode_status != UIMDIAG_SUCCESS)
  {
    return;
  }
  if(uimdiag_offset + sizeof(uint8) >= uimdiag_req_len)
  {
    UIMDIAG_MSG_ERROR_0("uimdiag_mmgsdi_util_decode_uint8- Cmd error");
    uimdiag_decode_status = UIMDIAG_ERROR;
    return;
  }
  memscpy(cmd_ptr,
          sizeof(uint8),
          (void *)((uint8*)uimdiag_req_ptr + uimdiag_offset),
          sizeof(uint8));

  uimdiag_offset  += sizeof(uint8);
} /* uimdiag_mmgsdi_util_decode_uint8 */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_util_decode_uint32

DESCRIPTION
  This function copies uint32 from diag_req_ptr to cmd_ptr,
  from offset value.
  Before copy, it checks diag_req_len, to validate the data in diag_req_ptr.
  The offset is updated after the copy.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
static void uimdiag_mmgsdi_util_decode_uint32(void *cmd_ptr)
{
    if (uimdiag_decode_status != UIMDIAG_SUCCESS)
    {
      return;
    }
    if(uimdiag_offset + sizeof(uint32) >= uimdiag_req_len)
    {
      UIMDIAG_MSG_ERROR_0("uimdiag_mmgsdi_util_decode_uint32- Cmd error");
      uimdiag_decode_status = UIMDIAG_ERROR;
      return;
    }
    memscpy(cmd_ptr,
          sizeof(uint32),
          (void *)((uint8*)uimdiag_req_ptr + uimdiag_offset),
          sizeof(uint32));

    uimdiag_offset  += sizeof(uint32);
}

/*===========================================================================

FUNCTION uimdiag_mmgsdi_util_decode_len_val_uint8

DESCRIPTION
  This function dynamically allocates 'length' number of bytes in the cmd_ptr.
  It copes the 'length' bytes from diag_req_ptr to the allocated cmd_ptr,
  from offset value.
  Before copy, it checks diag_req_len, to validate the data in diag_req_ptr.
  The offset is updated after the copy.

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type, which reflects status of allocation.

SIDE EFFECTS
  None.
===========================================================================*/

static void
  uimdiag_mmgsdi_util_decode_len_val_uint8(
    uint8                                             ** cmd_val_ptr,
    uint32                                               length)
{
  if (uimdiag_decode_status != UIMDIAG_SUCCESS)
  {
    return;
  }
  if(uimdiag_offset + length >= uimdiag_req_len)
  {
    UIMDIAG_MSG_ERROR_0("uimdiag_mmgsdi_util_decode_len_val_uint8- Cmd error");
    uimdiag_decode_status = UIMDIAG_ERROR;
    return;
  }
  if(length > 0)
  {
    UIMDIAG_UTIL_TMC_MEM_MALLOC_AND_VALIDATE(
        *cmd_val_ptr,
        length);
    if(*cmd_val_ptr == NULL)
    {
      uimdiag_decode_status = UIMDIAG_MEMORY_ERROR_HEAP_EXHAUSTED;
      return;
    }
    memscpy(*cmd_val_ptr,
      (length* sizeof(uint8)),
      (void *)((uint8*)uimdiag_req_ptr + uimdiag_offset),
      (length* sizeof(uint8)));
  }

  if(length == 0)
  {
    *cmd_val_ptr = NULL;
  }

  uimdiag_offset  += (length)*sizeof(uint8);
}

/*===========================================================================

FUNCTION uimdiag_mmgsdi_util_decode_len_val_uint16

DESCRIPTION
  This function dynamically allocates 'length' number of bytes in the cmd_ptr.
  It copes the 'length' bytes from diag_req_ptr to the allocated cmd_ptr,
  from offset value.
  Before copy, it checks diag_req_len, to validate the data in diag_req_ptr.
  The offset is updated after the copy.

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type, which reflects status of allocation.

SIDE EFFECTS
  None.
===========================================================================*/

static void
  uimdiag_mmgsdi_util_decode_len_val_uint16(
    uint8                                             ** cmd_val_ptr,
    uint32                                               length)
{
  if (uimdiag_decode_status != UIMDIAG_SUCCESS)
  {
    return;
  }
  if(uimdiag_offset + length >= uimdiag_req_len)
  {
    UIMDIAG_MSG_ERROR_0("uimdiag_mmgsdi_util_decode_len_val_uint8- Cmd error");
    uimdiag_decode_status = UIMDIAG_ERROR;
    return;
  }
  if(length > 0)
  {
    UIMDIAG_UTIL_TMC_MEM_MALLOC_AND_VALIDATE(
        *cmd_val_ptr,
        (length*sizeof(uint16)));
    if(*cmd_val_ptr == NULL)
    {
      uimdiag_decode_status = UIMDIAG_MEMORY_ERROR_HEAP_EXHAUSTED;
      return;
    }
    memscpy(*cmd_val_ptr,
      (length* sizeof(uint16)),
      (void *)((uint8*)uimdiag_req_ptr + uimdiag_offset),
      (length* sizeof(uint16)));
  }

  if(length == 0)
  {
    *cmd_val_ptr = NULL;
  }

  uimdiag_offset  += (length)*sizeof(uint16);
}

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_OPEN_CMD

DESCRIPTION
  This function parses cmd_ptr and fills the session open command params

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_session_open_cmd(
  uimdiag_mmgsdi_session_open_cmd_type         * cmd_ptr,
  const void                                   * diag_req_ptr,
  uint32                                       diag_req_len)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1. Client Id Lower/Higher 32 bits
  2. Session Type
  3. Set Notify
  4. Exclusive Channel
  5. AID Data Len
  6. AID Data
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_high);

  /* offset updated to access Client ID Lower 32 bits */

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_low);

  /* offset updated to access the session type */

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_type);

  /* offset updated to access exclusive channel information */

  uimdiag_mmgsdi_util_decode_uint32((void*)
    &cmd_ptr->exclusive_channel);

  /* offset updated to access the set notify field */

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->set_notify);

  /* offset updated to get AID length */

  uimdiag_mmgsdi_util_decode_uint32((void*)
    &cmd_ptr->aid_data_len);

  /* offset updated to access the AID Data Pointer */
  uimdiag_mmgsdi_util_decode_len_val_uint8(
     (uint8 **)&(cmd_ptr->aid_data_ptr),
           cmd_ptr->aid_data_len);

  /* prov session will have len as zero */
  if(cmd_ptr->aid_data_len == 0)
  {
     UIMDIAG_MSG_HIGH_0("Session open for Prov App, 0, 0");
  }

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_open_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_ID

DESCRIPTION
  This function parses the cmd_ptr and fills the session id params
  Functions which are using this are :
  1. session_close
  2. session_act_or_sw_prov
  3. session_get_all_pin_status
  4. uimdiag_mmgsdi_util_populate_session_deact_prov_cmd,
  5. uimdiag_mmgsdi_util_populate_session_get_app_cap_cmd,
  6. uimdiag_mmgsdi_util_populate_session_get_cphs_info_cmd,
  7. uimdiag_mmgsdi_util_populate_session_create_pkcs15_lookup_table_cmd

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_session_id(
  uimdiag_mmgsdi_sessionid_type           * cmd_ptr,
  const void                              * diag_req_ptr,
  uint32                                    diag_req_len)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1.Session Id Lower/Higher 32 bits
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to end of cmd_type */
  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_id */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_NVRUIM_DATA_SYNC

DESCRIPTION
  This function parses the cmd_ptr and fills the session id
  and NVRUIM data request params

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_nvruim_data_sync(
  uimdiag_mmgsdi_nvruim_data_req_type     * cmd_ptr,
  const void                              * diag_req_ptr,
  uint32                                    diag_req_len)
{
  UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1.Session Id Lower/Higher 32 bits
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* NVRUIM data request type */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->nvruim_data_req_enum);

  /* offset updated to end of cmd_type */
  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_nvruim_data_sync */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_CLIENT_ID_AND_EVT_DEREG_CMD

DESCRIPTION
  This function parses the cmd_ptr and fills the client id and evt dereg command params

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_client_id_and_evt_dereg_cmd(
  uimdiag_mmgsdi_client_id_and_evt_dereg_cmd_type   * cmd_ptr,
  const void                              * diag_req_ptr,
  uint32                                    diag_req_len)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1.Client Id Lower/Higher 32 bits
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_high);

  /* offset updated to access Client ID lower 32 bits */

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_low);

  /* offset updated to end of client_id_and_evt_dereg_cmd_type */

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_client_id_and_evt_dereg_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_ACT_OR_SW_PROV_CMD

DESCRIPTION
  This function parses the cmd_ptr and fills the session activate or switch
  command params

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_session_act_or_sw_prov_cmd(
  uimdiag_mmgsdi_session_act_or_sw_prov_cmd_type              * cmd_ptr,
  const void                                                  * diag_req_ptr,
  uint32                                                        diag_req_len)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1.Session Id Lower/Higher 32 bits
  2. slot Id
  3. AID Data Length
  4. AID Data Pointer
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to access the slot id */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->slot_id);

  /* offset updated to access AID Data length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->data_len);

  /* offset updated to point to AID Data Ptr */
  uimdiag_mmgsdi_util_decode_len_val_uint8(
     (uint8 **)&(cmd_ptr->data_ptr),
         cmd_ptr->data_len);
  if(cmd_ptr->data_len == 0)
  {
    cmd_ptr->data_ptr = NULL;
    UIMDIAG_MSG_ERROR_0("Session Activate and Switch Prov, Data Len is 0");
    return UIMDIAG_ERROR;
  }

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_act_or_sw_prov_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_GET_INFO_CMD

DESCRIPTION
  This function parses the cmd_ptr and allows a client to query MMGSDI for
  session information by Session ID or Session Type. A client can get
  Session Type, Slot Number, and AID, if querying by Session ID;
  or Session ID, Slot Number and AID, if querying by Session Type.

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_session_get_info_cmd(
  uimdiag_mmgsdi_session_get_info_cmd_type   * cmd_ptr,
  const void                                 * diag_req_ptr,
  uint32                                       diag_req_len)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1. Session Id Lower/Higher 32 bits
  2. Session Type
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* Query type */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_info);

  /* Session type */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_type);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_get_info_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_VERIFY_PIN_CMD

DESCRIPTION
  This command is used to perform a PIN verification procedure against a card
  for PIN1, PIN2 or UPIN.

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_session_verify_pin_cmd(
  uimdiag_mmgsdi_session_verify_pin_cmd_type           * cmd_ptr,
  const void                                           * diag_req_ptr,
  uint32                                                 diag_req_len)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1.Session Id Lower/Higher 32 bits
  2. Pin ID
  3. Pin Value
  4. Pin Value Ptr
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to access Pin ID */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->pin_id);

  /* offset updated to access length of PIN_VALUE */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->pin_len);

  /* offset updated to point to PIN VALUE Ptr */
  if(cmd_ptr->pin_len == 0)
  {
    cmd_ptr->pin_value_ptr = NULL;
    UIMDIAG_MSG_ERROR_0("Session verify pin, Pin Len is 0");
    return UIMDIAG_ERROR;
  }
  uimdiag_mmgsdi_util_decode_len_val_uint8(
     (uint8 **)&(cmd_ptr->pin_value_ptr),
      cmd_ptr->pin_len);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_verify_pin_status_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_DISABLE_PIN_CMD

DESCRIPTION
  This command is used to build a request to MMGSDI to disable the PIN provided

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_session_disable_pin_cmd(
  uimdiag_mmgsdi_session_disable_pin_cmd_type           * cmd_ptr,
  const void                                           * diag_req_ptr,
  uint32                                                 diag_req_len)
  {

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1.Session Id Lower/Higher 32 bits
  2. Pin ID
  3. Replace PIN
  4. Pin Value
  5. Pin Value Ptr
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to access Pin ID */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->pin_id);

  /* offset updated to access REPLACE_PIN */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->pin_replace);

  /* offset updated to access length of PIN_VALUE */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->pin_len);

  /* offset updated to point to PIN VALUE Ptr */

  if(cmd_ptr->pin_len == 0)
  {
    cmd_ptr->pin_value_ptr = NULL;
    UIMDIAG_MSG_ERROR_0("Session disable pin, Pin length is 0");
    return UIMDIAG_ERROR;
  }

  uimdiag_mmgsdi_util_decode_len_val_uint8(
     (uint8 **)&(cmd_ptr->pin_value_ptr),
      cmd_ptr->pin_len);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_disable_pin_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_ENABLE_PIN_CMD

DESCRIPTION
  This command is used to build a request to MMGSDI to enable the PIN provided

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_session_enable_pin_cmd(
  uimdiag_mmgsdi_session_enable_pin_cmd_type           * cmd_ptr,
  const void                                           * diag_req_ptr,
  uint32                                                 diag_req_len)
  {

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1.Session Id Lower/Higher 32 bits
  2. Pin Id
  3. Pin Value
  4. Pin Value Ptr
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to access Pin ID */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->pin_id);

  /* offset updated to access length of PIN_VALUE */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->pin_len);

  /* offset updated to point to PIN VALUE Ptr */
  if(cmd_ptr->pin_len == 0)
  {
    cmd_ptr->pin_value_ptr = NULL;
    UIMDIAG_MSG_ERROR_0("Session Enable pin, Pin length is 0");
    return UIMDIAG_ERROR;
  }

  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->pin_value_ptr),
     cmd_ptr->pin_len);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_enable_pin_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_CHANGE_PIN_CMD

DESCRIPTION
  This command is used to build a request to MMGSDI to change the PIN provided

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_session_change_pin_cmd(
  uimdiag_mmgsdi_session_change_pin_cmd_type           * cmd_ptr,
  const void                                           * diag_req_ptr,
  uint32                                                 diag_req_len)
  {

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1. Session Id Lower/Higher 32 bits
  2. Pin ID
  3. Pin Value
  4. Pin Value Ptr
  5. New Pin Value
  6. New Pin Value Ptr
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to access Pin ID */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->pin_id);

  /* offset updated to access length of PIN_VALUE */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->pin_len);

  /* offset updated to point to PIN VALUE Ptr */
  if(cmd_ptr->pin_len == 0)
  {
    cmd_ptr->pin_value_ptr = NULL;
    return UIMDIAG_ERROR;
  }
  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->pin_value_ptr),
    cmd_ptr->pin_len);

  /* offset updated to access length of NEW_PIN_VALUE */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->new_pin_len);

  /* offset updated to point to NEW_PIN_VALUE Ptr */
  if(cmd_ptr->new_pin_len == 0)
  {
    cmd_ptr->new_pin_value_ptr = NULL;
    UIMDIAG_MSG_ERROR_0("Session Change pin, Pin length is 0");
    return UIMDIAG_ERROR;
  }
  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->new_pin_value_ptr),
     cmd_ptr->new_pin_len);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_change_pin_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_UNBLOCK_PIN_CMD

DESCRIPTION
  This command is used to build a request to MMGSDI to unblock the PIN provided

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_session_unblock_pin_cmd(
  uimdiag_mmgsdi_session_unblock_pin_cmd_type           * cmd_ptr,
  const void                                           * diag_req_ptr,
  uint32                                                 diag_req_len)
  {

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1. Session Id Lower/Higher 32 bits
  2. Pin Id
  3. Puk Value
  4. Puk Value Ptr
  5. New Pin Value
  6. New Pin Value Ptr
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to access Pin ID */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->pin_id);

  /* offset updated to access length of PUK_VALUE */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->puk_len);

  /* offset updated to point to PUK VALUE Ptr */
  if(cmd_ptr->puk_len == 0)
  {
    cmd_ptr->puk_value_ptr = NULL;
    return UIMDIAG_ERROR;
  }
  uimdiag_mmgsdi_util_decode_len_val_uint8(
       (uint8 **)&(cmd_ptr->puk_value_ptr),
        cmd_ptr->puk_len);

  /* offset updated to access length of NEW_PIN_VALUE */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->new_pin_len);

  /* offset updated to point to NEW_PIN_VALUE Ptr */
  if(cmd_ptr->new_pin_len == 0)
  {
    cmd_ptr->new_pin_value_ptr = NULL;
    UIMDIAG_MSG_ERROR_0("Session Unblock pin, Pin length is 0");
    return UIMDIAG_ERROR;
  }
  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->new_pin_value_ptr),
    cmd_ptr->new_pin_len);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_unblock_pin_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_GET_FILE_ATTR_CMD

DESCRIPTION
  This command is used to build a request to MMGSDI to get file attr

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_session_get_file_attr_cmd(
  uimdiag_mmgsdi_session_get_file_attr_cmd_type          * cmd_ptr,
  const void                                           * diag_req_ptr,
  uint32                                                 diag_req_len)
  {

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1.Session Id Lower/Higher 32 bits
  2.Access method
  3.ef enum
  4.df enum
  5.path len and path
  6.aid len and aid
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to access "access_method" */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->access_method);

  /* offset updated to access ef enum */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->ef_enum);

  /* offset updated to access df enum */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->df_enum);

  /* offset updated to get path length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->path_len);

  /* offset updated to access the path */
  uimdiag_mmgsdi_util_decode_len_val_uint16(
    (uint8 **)&(cmd_ptr->path_ptr),
           cmd_ptr->path_len);

  /* aid_len */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->aid_len);

  /* offset updated to access the AID Data Pointer */
  if(cmd_ptr->aid_len == 0)
  {
    cmd_ptr->aid_ptr = NULL;
    UIMDIAG_MSG_ERROR_0("Session Get File Attr, AID length is 0");
    return UIMDIAG_ERROR;
  }
  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->aid_ptr),
    cmd_ptr->aid_len);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_get_file_attr_cmd */

/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_READ_RECORD_CMD

DESCRIPTION
  This command is used to build a request to MMGSDI to read record

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_session_read_record_cmd(
  uimdiag_mmgsdi_session_read_record_cmd_type          * cmd_ptr,
  const void                                           * diag_req_ptr,
  uint32                                                 diag_req_len)
  {

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1.Session Id Lower/Higher 32 bits
  2.Access method
  3.ef enum
  4.df enum
  5.path len and path
  6.aid len and aid
  7.record number
  8.record length
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to access "access_method" */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->access_method);

  /* offset updated to access ef enum */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->ef_enum);

  /* offset updated to access df enum */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->df_enum);

  /* offset updated to get path length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->path_len);

  /* offset updated to access the path */
  uimdiag_mmgsdi_util_decode_len_val_uint16(
    (uint8 **)&(cmd_ptr->path_ptr),
      cmd_ptr->path_len);

  /* aid_len */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->aid_len);

  if(cmd_ptr->aid_len == 0)
  {
    cmd_ptr->aid_ptr = NULL;
    return UIMDIAG_ERROR;
  }

  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->aid_ptr),
    cmd_ptr->aid_len);

  /* rec_num */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->rec_num);

  /* offset updated to get record length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->rec_len);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_read_record_cmd */

/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_READ_TRANSPARENT_CMD

DESCRIPTION
  This command is used to build a request to MMGSDI to read transparent file

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_session_read_transparent_cmd(
  uimdiag_mmgsdi_session_read_transparent_cmd_type          * cmd_ptr,
  const void                                           * diag_req_ptr,
  uint32                                                 diag_req_len)
  {

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1.Session Id Lower/Higher 32 bits
  2.Access method
  3.ef enum
  4.df enum
  5.path len and path
  6.aid len and aid
  7.offset
  8.record length
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to access "access_method" */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->access_method);

  /* offset updated to access ef enum */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->ef_enum);

  /* offset updated to access df enum */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->df_enum);

  /* offset updated to get path length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->path_len);

  /* offset updated to access the path */
  uimdiag_mmgsdi_util_decode_len_val_uint16(
    (uint8 **)&(cmd_ptr->path_ptr),
     cmd_ptr->path_len);

  /* aid_len */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->aid_len);

  /* offset updated to access the AID Data Pointer */
  if(cmd_ptr->aid_len == 0)
  {
    cmd_ptr->aid_ptr = NULL;
    return UIMDIAG_ERROR;
  }

  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->aid_ptr),
    cmd_ptr->aid_len);

  /* offset */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->offset);

  /* offset updated to get record length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->req_len);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_read_transparent_cmd */

/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_WRITE_RECORD_CMD

DESCRIPTION
  This command is used to build a request to MMGSDI to write record

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_session_write_record_cmd(
  uimdiag_mmgsdi_session_write_record_cmd_type          * cmd_ptr,
  const void                                           * diag_req_ptr,
  uint32                                                 diag_req_len)
  {

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1.Session Id Lower/Higher 32 bits
  2.Access method
  3.ef enum
  4.df enum
  5.path len and path
  6.aid len and aid
  7.record type
  8.record number
  9.data length and data
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to access "access_method" */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->access_method);

  /* offset updated to access ef enum */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->ef_enum);

  /* offset updated to access df enum */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->df_enum);

  /* offset updated to get path length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->path_len);

  /* offset updated to access the path */
  uimdiag_mmgsdi_util_decode_len_val_uint16(
    (uint8 **)&(cmd_ptr->path_ptr),
           cmd_ptr->path_len);

  /* aid_len */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->aid_len);

  /* offset updated to access the AID Data Pointer */
  if(cmd_ptr->aid_len == 0)
  {
    cmd_ptr->aid_ptr = NULL;
    return UIMDIAG_ERROR;
  }

  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->aid_ptr),
    cmd_ptr->aid_len);

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->rec_type);

  /* offset updated to get record number */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->rec_num);

   /* offset updated to get data length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->data_len);

  /* offset updated to access the data Pointer */

  if(cmd_ptr->data_len == 0)
  {
    cmd_ptr->data_ptr = NULL;
    return UIMDIAG_ERROR;
  }

  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->data_ptr),
    cmd_ptr->data_len);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_write_record_cmd */


/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_WRITE_TRANSPARENT_CMD

DESCRIPTION
  This command is used to build a request to MMGSDI to write to transparent file.

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_session_write_transparent_cmd(
  uimdiag_mmgsdi_session_write_transparent_cmd_type    * cmd_ptr,
  const void                                           * diag_req_ptr,
  uint32                                                 diag_req_len)
  {

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1.Session Id Lower/Higher 32 bits
  2.Access method
  3.ef enum
  4.df enum
  5.path len and path
  6.aid len and aid
  7.offset
  8.data length and data
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to access "access_method" */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->access_method);

  /* offset updated to access ef enum */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->ef_enum);

  /* offset updated to access df enum */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->df_enum);

  /* offset updated to get path length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->path_len);

  /* offset updated to access the path */
  uimdiag_mmgsdi_util_decode_len_val_uint16(
    (uint8 **)&(cmd_ptr->path_ptr),
           cmd_ptr->path_len);

  /* aid_len */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->aid_len);

  /* offset updated to access the AID Data Pointer */
  if(cmd_ptr->aid_len == 0)
  {
    cmd_ptr->aid_ptr = NULL;
    return UIMDIAG_ERROR;
  }

  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->aid_ptr),
    cmd_ptr->aid_len);

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->offset);

  /* offset updated to get data length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->data_len);

  /* offset updated to access the data Pointer */
  if(cmd_ptr->data_len == 0)
  {
    cmd_ptr->data_ptr = NULL;
    return UIMDIAG_ERROR;
  }

  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->data_ptr),
    cmd_ptr->data_len);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_write_transparent_cmd */

/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_REFRESH_CMD

DESCRIPTION
  This command is used to build a request to register / deregister
  for refresh

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_session_refresh_cmd(
    uimdiag_mmgsdi_session_refresh_cmd_type              * cmd_ptr,
    const void                                           * diag_req_ptr,
    uint32                                                 diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1.Session Id Lower/Higher 32 bits
  2.File List length
  3.File List
  4.File Path length
  5.File path
  6.Vote for Init

  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to access file list length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->num_of_files);

  /* offset updated to access file_list */
  /* Note file_list_ptr is uint32*        */
  UIMDIAG_ASSERT((uimdiag_offset <= diag_req_len), UIMDIAG_ERROR);

  if(cmd_ptr->num_of_files > 0)
  {
    UIMDIAG_UTIL_TMC_MEM_MALLOC_AND_VALIDATE(
      cmd_ptr->file_list_ptr,
      (cmd_ptr->num_of_files * sizeof(uint32)));

    if(cmd_ptr->file_list_ptr == NULL)
    {
      uimdiag_decode_status = UIMDIAG_MEMORY_ERROR_HEAP_EXHAUSTED;
      return uimdiag_decode_status;
    }

    UIMDIAG_ASSERT(((uimdiag_offset + cmd_ptr->num_of_files) <= diag_req_len), UIMDIAG_ERROR);
    memscpy((void *)cmd_ptr->file_list_ptr,
      (cmd_ptr->num_of_files * sizeof(uint32)),
      (void *)((uint8*)diag_req_ptr + uimdiag_offset),
      (cmd_ptr->num_of_files* sizeof(uint32)));
  }

  if(cmd_ptr->num_of_files == 0)
  {
    cmd_ptr->file_list_ptr = NULL;
  }

  uimdiag_offset  += (cmd_ptr->num_of_files*sizeof(uint32));

  /* offset updated to access file path length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->file_path_len);

  /* offset updated to get file path */
  uimdiag_mmgsdi_util_decode_len_val_uint16(
    (uint8 **)&(cmd_ptr->file_path_ptr),
      cmd_ptr->file_path_len);

  /* offset being updated to point to vote for init */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->vote_for_init);

  /* offset updated to the end of the data structure */
  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_refresh_cmd */


/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_REFRESH_PROCEED_VOTING_CMD

DESCRIPTION
  This command is used to build a request to register / deregister
  for refresh and also to receive a notification from MMGSDI before proceeding with Refresh

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_session_refresh_proceed_voting_cmd(
    uimdiag_mmgsdi_session_refresh_proceed_voting_cmd_type   * cmd_ptr,
    const void                                               * diag_req_ptr,
    uint32                                                     diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1.Session Id Lower/Higher 32 bits
  2.File List length
  3.File List
  4.File Path length
  5.File path
  6.Vote for Init

  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to access file list length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->num_of_files);

  /* offset updated to access file_list */
  /* Note file_list_ptr is uint32*        */
  UIMDIAG_ASSERT((uimdiag_offset <= diag_req_len), UIMDIAG_ERROR);

  if(cmd_ptr->num_of_files > 0)
  {
    UIMDIAG_UTIL_TMC_MEM_MALLOC_AND_VALIDATE(
      cmd_ptr->file_list_ptr,
      (cmd_ptr->num_of_files * sizeof(uint32)));

    if(cmd_ptr->file_list_ptr == NULL)
    {
      uimdiag_decode_status = UIMDIAG_MEMORY_ERROR_HEAP_EXHAUSTED;
      return uimdiag_decode_status;
    }

    UIMDIAG_ASSERT(((uimdiag_offset + cmd_ptr->num_of_files) <= diag_req_len), UIMDIAG_ERROR);
    memscpy((void *)cmd_ptr->file_list_ptr,
      (cmd_ptr->num_of_files * sizeof(uint32)),
      (void *)((uint8*)diag_req_ptr + uimdiag_offset),
      (cmd_ptr->num_of_files* sizeof(uint32)));
  }

  if(cmd_ptr->num_of_files == 0)
  {
    cmd_ptr->file_list_ptr = NULL;
  }

  uimdiag_offset  += (cmd_ptr->num_of_files*sizeof(uint32));

  /* offset updated to access file path length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->file_path_len);

  /* offset updated to get file path */
  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->file_path_ptr),
      cmd_ptr->file_path_len);

  /* offset being updated to point to vote for init */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->vote_for_init);

  /* offset being updated to point to ok to proceed */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->proceed_refresh);

  /* offset updated to the end of the data structure */
  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_refresh_proceed_voting_cmd */


/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_NAA_REFRESH_CMD

DESCRIPTION
  This command is used to build a request to trigger for refresh

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_session_naa_refresh_cmd(
    uimdiag_mmgsdi_naa_refresh_cmd_type   * cmd_ptr,
    const void                            * diag_req_ptr,
    uint32                                  diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);


  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1. Client ID Id Lower/Higher 32 bits
  2. Slot ID.
  3. App ID Length
  4. App ID
  5. Refresh Mode
  6. Path Length
  7. Path

  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_high);
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_low);

   /* offset updated to access slot ID  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->slot_id);


  /* offset updated to access App Id Length  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->sim_app_id_len);

  /* offset updated to access aid*/
  /* Note sim_app_id is uint32*        */
  UIMDIAG_ASSERT((uimdiag_offset <= diag_req_len), UIMDIAG_ERROR);

  if(cmd_ptr->sim_app_id_len > 0)
  {
    UIMDIAG_UTIL_TMC_MEM_MALLOC_AND_VALIDATE(
      cmd_ptr->sim_app_id,
      (cmd_ptr->sim_app_id_len * sizeof(uint8)));
    if(cmd_ptr->sim_app_id == NULL)
    {
      uimdiag_decode_status = UIMDIAG_ERROR;
      return uimdiag_decode_status;
    }

    UIMDIAG_ASSERT(((uimdiag_offset + cmd_ptr->sim_app_id_len) <= diag_req_len), UIMDIAG_ERROR);
    memscpy((void *)cmd_ptr->sim_app_id,
      (cmd_ptr->sim_app_id_len * sizeof(uint8)),
      (void *)((uint8*)diag_req_ptr + uimdiag_offset),
      (cmd_ptr->sim_app_id_len* sizeof(uint8)));
  }

  if(cmd_ptr->sim_app_id_len == 0)
  {
    cmd_ptr->sim_app_id = NULL;
  }

  uimdiag_offset  += (cmd_ptr->sim_app_id_len*sizeof(uint8));

  /* offset being updated to point to refresh mode */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->refresh_mode);

  /* offset being updated to point to path Length  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->path_len);

  /* offset updated to path */
  /* Note path length is uint8*        */
  UIMDIAG_ASSERT((uimdiag_offset <= diag_req_len), UIMDIAG_ERROR);

  if(cmd_ptr->path_len > 0)
  {
    UIMDIAG_UTIL_TMC_MEM_MALLOC_AND_VALIDATE(
      cmd_ptr->path,
      (cmd_ptr->path_len * sizeof(uint8)));

    if(cmd_ptr->path == NULL)
    {
      uimdiag_decode_status = UIMDIAG_ERROR;
      return uimdiag_decode_status;
    }

    UIMDIAG_ASSERT(((uimdiag_offset + cmd_ptr->path_len) <= diag_req_len), UIMDIAG_ERROR);
    memscpy((void *)cmd_ptr->path,
      (cmd_ptr->path_len * sizeof(uint8)),
      (void *)((uint8*)diag_req_ptr + uimdiag_offset),
      (cmd_ptr->path_len* sizeof(uint8)));
  }

  if(cmd_ptr->path_len == 0)
  {
    cmd_ptr->path = NULL;
  }

  uimdiag_offset  += (cmd_ptr->path_len*sizeof(uint8));

  /* offset updated to the end of the data structure */
  return uimdiag_decode_status;

}/* uimdiag_mmgsdi_util_populate_session_naa_refresh_cmd */


/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_OK_TO_REFRESH_CMD

DESCRIPTION
  This command is used to build a request to simulate an OK to Refresh to MMGSDI.

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_session_ok_to_refresh_cmd (
    uimdiag_mmgsdi_session_ok_to_refresh_cmd_type        * cmd_ptr,
    const void                                           * diag_req_ptr,
    uint32                                                 diag_req_len )
{

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1.Session Id Lower/Higher 32 bits
  2.Ok_to_refresh

  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to access ok_to_refresh field */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->ok_to_refresh);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_ok_to_refresh_cmd */

/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_PROCEED_WITH_REFRESH_CMD

DESCRIPTION
  This command is used to build a request to simulate an proceed with refresh to MMGSDI.

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_session_proceed_with_refresh_cmd (
    uimdiag_mmgsdi_session_proceed_refresh_cmd_type      * cmd_ptr,
    const void                                           * diag_req_ptr,
    uint32                                                 diag_req_len )
{

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1.Session Id Lower/Higher 32 bits
  2.Ok_to_refresh

  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to access ok_to_refresh field */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->ok_to_proceed);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_proceed_with_refresh_cmd */

/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_REFRESH_COMPLETE_CMD

DESCRIPTION
  This command is used to build a request to to indicate to MMGSDI that the
  client has finished its refresh process

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_session_refresh_complete_cmd(
    uimdiag_mmgsdi_session_refresh_complete_cmd_type     * cmd_ptr,
    const void                                           * diag_req_ptr,
    uint32                                                 diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1.Session Id Lower/Higher 32 bits
  2.Pass_fail
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to access pass or fail refresh field */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->pass_fail);

  return UIMDIAG_SUCCESS;
}/* uimdiag_mmgsdi_util_populate_session_refresh_complete_cmd */

/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_PERSO_CMD

DESCRIPTION
  This command is used to build a request to to indicate to MMGSDI to activate
  / deactivate a personalization feature by an external source.
  This command is also used to retrieve the information about personalization
  Indicators. This command is used to set the personalization data for a
  particular personalization feature

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_session_perso_cmd(
    uimdiag_mmgsdi_session_perso_cmd_type         * cmd_ptr,
    const void                                    * diag_req_ptr,
    uint32                                          diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer
  1. Session Id Lower/Higher 32 bits
  2. Feature Name
  3. Perso Client Data Length
  4. Perso Client Data
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to access the feature name */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->feature_name);

  /* offset updated to get the perso client data length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->data_len);

  /* offset updated to get perso client data */
  if(cmd_ptr->data_len == 0)
  {
    cmd_ptr->data_ptr = NULL;
    return UIMDIAG_ERROR;
  }
  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->data_ptr),
    cmd_ptr->data_len);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_perso_cmd */

/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_PERSO_GET_IND_CMD

DESCRIPTION
  This command is used to build a request to retrieve the
  information about personalization Indicators

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_session_perso_get_ind_cmd(
    uimdiag_mmgsdi_session_perso_get_ind_cmd_type * cmd_ptr,
    const void                                    * diag_req_ptr,
    uint32                                          diag_req_len)
    {

      UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
      UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);

      /* -----------------------------------------------------
      Parse following fields from diag buffer

      1. Session Id Lower/Higher 32 bits
      2. Identifier to get all or permanently disabled feature indicators

      ----------------------------------------------------- */
      uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

   /* Session ID higher 32 bits  */
      uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

   /* Session ID lower 32 bits  */
      uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

      /* offset updated to access the feature name */
      uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->perso_feat_ind);
      /* offset updated to the end of the data structure */

      return uimdiag_decode_status;
 }/* uimdiag_mmgsdi_util_populate_session_perso_get_ind_cmd */

/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_PERSO_GET_DATA_CMD

DESCRIPTION
  This command is used to build a request to retrieve personalization data.

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_session_perso_get_data_cmd(
    uimdiag_mmgsdi_session_perso_cmd_type         * cmd_ptr,
    const void                                    * diag_req_ptr,
    uint32                                          diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer
  1. Session Id Lower/Higher 32 bits
  2. Feature Name
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to access the feature name */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->feature_name);
  /* offset updated to the end of the data structure */

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_perso_get_data_cmd */

/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_PERSO_GET_KEY_CMD

DESCRIPTION
  This command is used to get the personalization key

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_session_perso_get_key_cmd(
    uimdiag_mmgsdi_session_perso_get_key_cmd_type * cmd_ptr,
    const void                                    * diag_req_ptr,
    uint32                                          diag_req_len)
{

      UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
      UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);

      /* -----------------------------------------------------
      Parse following fields from diag buffer

      1. Session Id Lower/Higher 32 bits
      2. Personalization feature
      3. Perso Key type

      ----------------------------------------------------- */
      uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);
      /* Session ID higher 32 bits  */
      uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

      /* Session ID lower 32 bits  */
      uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

      /* offset updated to access the feature identifier */
      uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->feature);

      /* offset updated to access the key type */
      uimdiag_mmgsdi_util_decode_uint32((void*) &cmd_ptr->key_type);

      /* offset updated to the end of the data structure */

      return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_perso_get_key_cmd */


/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_PERSO_GET_DCK_RETRIES_CMD

DESCRIPTION
  This command is used to get the number of depersonalization attempts
  remaining

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_session_perso_get_dck_retries_cmd(
    uimdiag_mmgsdi_session_perso_get_dck_retries_cmd_type * cmd_ptr,
    const void                                            * diag_req_ptr,
    uint32                                                  diag_req_len)
{
      uint32                        offset         = 0;

      UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
      UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);

      /* -----------------------------------------------------
      Parse following fields from diag buffer

      1. Session Id Lower/Higher 32 bits
      2. Dck_retries_type
      ----------------------------------------------------- */
      UIMDIAG_ASSERT((sizeof(uint32) < diag_req_len), UIMDIAG_ERROR);
      memscpy((void *)&cmd_ptr->session_id_high,
        sizeof(cmd_ptr->session_id_high),
        (void *)((uint8*)diag_req_ptr + offset),
        sizeof(uint32));

      offset  += sizeof(uint32);
      /* offset updated to access Session ID lower 32 bits */

      UIMDIAG_ASSERT(((offset + sizeof(uint32)) <= diag_req_len), UIMDIAG_ERROR);
      memscpy((void *)&cmd_ptr->session_id_low,
        sizeof(cmd_ptr->session_id_low),
        (void *)((uint8*)diag_req_ptr + offset),
        sizeof(uint32));

      offset  += sizeof(uint32);
      /* offset updated to access the DCK Retries identifier */

      UIMDIAG_ASSERT(((offset + sizeof(uint32)) <= diag_req_len), UIMDIAG_ERROR);
      memscpy((void *)&cmd_ptr->dck_retries_type,
        sizeof(cmd_ptr->dck_retries_type),
        (void *)((uint8*)diag_req_ptr + offset),
        sizeof(uint32));

      offset  += sizeof(uint32);
      /* offset updated to the end of the data structure */
      UIMDIAG_ASSERT((offset <= diag_req_len), UIMDIAG_ERROR);

      return UIMDIAG_SUCCESS;
}/* uimdiag_mmgsdi_util_populate_session_perso_get_dck_retries_cmd */


/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_PERSO_LOCK_DWN_ENABLE_LTD_ACCESS_CMD

DESCRIPTION
  This command is used to used to lock down some of the perso commands.
  remaining

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_session_perso_lock_dwn_enable_ltd_access_cmd(
    uimdiag_mmgsdi_session_perso_lock_dwn_enable_ltd_access_cmd_type
                                                          * cmd_ptr,
    const void                                            * diag_req_ptr,
    uint32                                                  diag_req_len)
{
      uint32                        offset         = 0;

      UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
      UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);

      /* -----------------------------------------------------
      Parse following fields from diag buffer

      1. Session Id Lower/Higher 32 bits
      2. locked_command
      ----------------------------------------------------- */
      UIMDIAG_ASSERT((sizeof(uint32) < diag_req_len), UIMDIAG_ERROR);
      memscpy((void *)&cmd_ptr->session_id_high,
        sizeof(cmd_ptr->session_id_high),
        (void *)((uint8*)diag_req_ptr + offset),
        sizeof(uint32));

      offset  += sizeof(uint32);
      /* offset updated to access Session ID lower 32 bits */

      UIMDIAG_ASSERT(((offset + sizeof(uint32)) <= diag_req_len), UIMDIAG_ERROR);
      memscpy((void *)&cmd_ptr->session_id_low,
        sizeof(cmd_ptr->session_id_low),
        (void *)((uint8*)diag_req_ptr + offset),
        sizeof(uint32));

      offset  += sizeof(uint32);
      /* offset updated to access the locked command */

      UIMDIAG_ASSERT(((offset + sizeof(uint32)) <= diag_req_len), UIMDIAG_ERROR);
      memscpy((void *)&cmd_ptr->action_locked,
        sizeof(cmd_ptr->action_locked),
        (void *)((uint8*)diag_req_ptr + offset),
        sizeof(uint32));

      offset  += sizeof(uint32);
      /* offset updated to the end of the data structure */
      UIMDIAG_ASSERT((offset <= diag_req_len), UIMDIAG_ERROR);

      return UIMDIAG_SUCCESS;
}/* uimdiag_mmgsdi_util_populate_session_perso_lock_dwn_enable_ltd_access_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_ISIM_AUTH_CMD

DESCRIPTION
  This function parses cmd_ptr and is used to erform a "Network" Initiated
  Authentication with the ISIM Application.

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_session_isim_auth_cmd(
  uimdiag_mmgsdi_session_isim_auth_cmd_type    * cmd_ptr,
  const void                                   * diag_req_ptr,
  uint32                                         diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1. Session Id Lower/Higher 32 bits
  2. Auth Challenge Len
  3. Auth challenge Data
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to get Auth Challenge length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->auth_data_len);

  /* offset updated to access the Auth Challenge Data Pointer */

  if(cmd_ptr->auth_data_len == 0)
  {
     cmd_ptr->auth_data_ptr = NULL;
     UIMDIAG_MSG_ERROR_0("Session ISIM Authenticate  Data length is zero, 0, 0");
     return UIMDIAG_ERROR;
  }

  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->auth_data_ptr),
    cmd_ptr->auth_data_len);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_isim_auth_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_SEND_CARD_STATUS_CMD

DESCRIPTION
  This function parses cmd_ptr and is used to get all information
  concerning the current directory

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_session_send_card_status_cmd(
    uimdiag_mmgsdi_session_send_crd_status_cmd_type    * cmd_ptr,
    const void                                         * diag_req_ptr,
    uint32                                               diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);

  /* ------------------------------------------------------------------
  Parse following fields from diag buffer

  1. Session Id:      Lower/Higher 32 bits
  2. me_app_status:   The status of the application in the handset
  3. ret_data_struct: Structure in which the data should be returned in
  ------------------------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to status of the application in the handset  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->me_app_status);

  /* offset updated to Structure in which the data should be returned in  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->ret_data_struct);

    /* offset being updated to point to the end of session send card status command */

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_send_card_status_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_SERVICE

DESCRIPTION
  This function parses cmd_ptr and is used to get all information
  related enable/ disable service, is service available

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_session_service(
    uimdiag_mmgsdi_session_service_cmd_type            * cmd_ptr,
    const void                                         * diag_req_ptr,
    uint32                                               diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* ------------------------------------------------------------------
  Parse following fields from diag buffer

  1. Session Id:      Lower/Higher 32 bits
  2. service:         The service that needs disabled/ enabled
  ------------------------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to service type  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->service);

  /* offset being updated to point to the end of session send card status command */

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_service */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_SEEK_CMD

DESCRIPTION
  This function parses cmd_ptr and is used to get all information
  related to UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_SEEK_CMD

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_session_seek_cmd(
  uimdiag_mmgsdi_session_seek_cmd_type          * cmd_ptr,
  const void                                    * diag_req_ptr,
  uint32                                          diag_req_len)
{
  UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1.Session Id Lower/Higher 32 bits
  2.Access method
  3.ef enum
  4.df enum
  5.path len and path
  6.aid len and aid
  7.seek direction
  8.seek pattern
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* offset updated to access Session ID higher 32 bits */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* offset updated to access Session ID lower 32 bits */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to access "access_method" */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->access_method);

  /* offset updated to access ef enum */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->ef_enum);

  /* offset updated to access df enum */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->df_enum);

  /* offset updated to get path length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->path_len);

  /* offset updated to access the path */
  uimdiag_mmgsdi_util_decode_len_val_uint16(
    (uint8 **)&(cmd_ptr->path_ptr),
           cmd_ptr->path_len);

  /* aid_len */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->aid_len);

  /* offset updated to access the AID Data Pointer */
  if(cmd_ptr->aid_len == 0)
  {
    cmd_ptr->aid_ptr = NULL;
    UIMDIAG_MSG_ERROR_0("POPULATE SESSION SEEK CMD, AID length is 0");
    return UIMDIAG_ERROR;
  }

  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->aid_ptr),
    cmd_ptr->aid_len);

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->seek_direction);

  /* offset updated to access "seek pattern" */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->seek_pattern_len);

  /* offset updated to access the Seek Pattern Pointer */
  if(cmd_ptr->seek_pattern_len== 0)
  {
    cmd_ptr->seek_pattern_ptr = NULL;
    UIMDIAG_MSG_ERROR_0("POPULATE SESSION SEEK CMD, Seek Pattern length is 0");
    return UIMDIAG_ERROR;
  }
  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->seek_pattern_ptr),
    cmd_ptr->seek_pattern_len);

  return UIMDIAG_SUCCESS;
}/* uimdiag_mmgsdi_util_populate_session_seek_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_SEARCH_CMD

DESCRIPTION
  This command performs Search command on UICC card.

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_session_search_cmd(
  uimdiag_mmgsdi_session_search_cmd_type        * cmd_ptr,
  const void                                    * diag_req_ptr,
  uint32                                          diag_req_len)
{
  UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1.  Session Id Lower/Higher 32 bits
  2.  Access method
  3.  ef enum
  4.  df enum
  5.  path len and path
  6.  aid len and aid
  7.  search_type
  8.  search_record_num
  9.  search_direction
  10. offset
  11. search_pattern
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* offset updated to access Session ID higher 32 bits */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* offset updated to access Session ID lower 32 bits */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to access "access_method" */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->access_method);

  /* offset updated to access ef enum */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->ef_enum);

  /* offset updated to access df enum */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->df_enum);

  /* offset updated to get path length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->path_len);

  /* offset updated to access the path */
  uimdiag_mmgsdi_util_decode_len_val_uint16(
    (uint8 **)&(cmd_ptr->path_ptr),
           cmd_ptr->path_len);

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->aid_len);

  /* offset updated to access the AID Data Pointer */
  if(cmd_ptr->aid_len == 0)
  {
    cmd_ptr->aid_ptr = NULL;
    UIMDIAG_MSG_ERROR_0("POPULATE SESSION SEARCH CMD, AID length is 0");
    return UIMDIAG_ERROR;
  }

  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->aid_ptr),
    cmd_ptr->aid_len);

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->search_type);

  /* offset updated to access search record number */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->search_record_num);

  /* offset updated to access search direction */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->search_direction);

  /* offset updated to access the offset_type */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->offset_type);

  /* offset updated to access the offset data */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->offset_data);

  /* offset updated to access the Search Pattern Length  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->search_pattern_len);

  /* offset updated to access the Search Pattern Data Pointer  */
  if(cmd_ptr->search_pattern_len == 0)
  {
    cmd_ptr->search_pattern_ptr = NULL;
    UIMDIAG_MSG_ERROR_0("POPULATE SESSION SEARCH CMD, Search Pattern length is 0");
  }
  else
  {
    uimdiag_mmgsdi_util_decode_len_val_uint8(
      (uint8 **)&(cmd_ptr->search_pattern_ptr),
      cmd_ptr->search_pattern_len);
  }

  return UIMDIAG_SUCCESS;
}/* uimdiag_mmgsdi_util_populate_session_search_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_PROV_APP_INIT_COMPLETE

DESCRIPTION
  This function parses cmd_ptr and is used to get all information
  related to UIMDIAG_MMGSDI_SESSION_PROV_APP_INIT_COMPLETE_CMD

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_app_init_complete(
    uimdiag_mmgsdi_session_prov_app_init_compl_cmd_type * cmd_ptr,
    const void                                         * diag_req_ptr,
    uint32                                               diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* ------------------------------------------------------------------
  Parse following fields from diag buffer

  1. Session Id:      Lower/Higher 32 bits
  2. init_status:       Initialization status of provisional App
  ------------------------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to init_status  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->init_status);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_app_init_complete */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_COMPUTE_IP_AUTH

DESCRIPTION
  This function parses cmd_ptr and is used to get all information
  related to UIMDIAG_MMGSDI_SESSION_COMPUTE_IP_AUTH

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_compute_ip_auth(
    uimdiag_mmgsdi_session_compute_ip_auth_cmd_type    * cmd_ptr,
    const void                                         * diag_req_ptr,
    uint32                                               diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);

  /* ------------------------------------------------------------------
  Parse following fields from diag buffer

  1. Session Id                :      Lower/Higher 32 bits
  2. operation_type         :   Type of authentication operation
  3. chap_id, chap_nai_entry_index, chap_chal_len, chap_challenge
                                     : CHAP operation for the SIP.
  4. mn_ha_nai_entry_index,mn_ha_reg_data_len,mn_ha_reg_data
                                     :  MN-HA authenticator.
  5. rrq_data_len,rrq_data : MIP-RRQ hash
  6. mn_aaa_nai_entry_index, mn_aaa_chal_len, mn_aaa_challenge
                                     : MN-AAA authenticator.
  7. hrpd_chap_id, hrpd_chap_chal_len, hrpd_chap_challenge
                                     : CHAP operation for High Rate Packet Data
  ------------------------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* operation_type */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->operation_type);

  /* chap_id  */
  uimdiag_mmgsdi_util_decode_uint8((void*)&cmd_ptr->chap_id);

  /* chap_nai_entry_index */
  uimdiag_mmgsdi_util_decode_uint8((void*)&cmd_ptr->chap_nai_entry_index);

  /* chap_chal_len */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->chap_chal_len);

  if(cmd_ptr->chap_chal_len > MMGSDI_MAX_3GPD_CHAP_CHALLENGE_LENGTH)
  {
     return UIMDIAG_ERROR;
  }

  /* chap_challenge */
  uimdiag_mmgsdi_util_decode_len_val_uint8(
   (uint8 **)&(cmd_ptr->chap_challenge),
    cmd_ptr->chap_chal_len);

  /* mn_ha_nai_entry_index */
  uimdiag_mmgsdi_util_decode_uint8((void*)&cmd_ptr->mn_ha_nai_entry_index);

  /* mn_ha_reg_data_len */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->mn_ha_reg_data_len);

  /* mn_ha_reg_data */
  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->mn_ha_reg_data),
    cmd_ptr->mn_ha_reg_data_len);

  /* rrq_data_len */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->rrq_data_len);

  /* rrq_data */
  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->rrq_data),
    cmd_ptr->rrq_data_len);

  /* mn_aaa_nai_entry_index */
  uimdiag_mmgsdi_util_decode_uint8((void*)&cmd_ptr->mn_aaa_nai_entry_index);

  /* mn_aaa_chal_len */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->mn_aaa_chal_len);

  /* mn_aaa_challenge */
  uimdiag_mmgsdi_util_decode_len_val_uint8(
      (uint8 **)&(cmd_ptr->mn_aaa_challenge),
       cmd_ptr->mn_aaa_chal_len);

  /* hrpd_chap_id */
  uimdiag_mmgsdi_util_decode_uint8((void*)&cmd_ptr->hrpd_chap_id);

  /* hrpd_chap_chal_len */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->hrpd_chap_chal_len);

  /* hrpd_chap_challenge */
  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->hrpd_chap_challenge),
     cmd_ptr->hrpd_chap_chal_len);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_app_init_complete */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_SSD_UPDATE

DESCRIPTION
  This function parses cmd_ptr and is used to get all information
  related to UIMDIAG_MMGSDI_SESSION_SSD_UPDATE

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_ssd_update(
    uimdiag_mmgsdi_session_ssd_update_cmd_type         * cmd_ptr,
    const void                                         * diag_req_ptr,
    uint32                                               diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);

  /* ------------------------------------------------------------------
  Parse following fields from diag buffer
  session_id_high:  Higher 32 bits of session ID
  session_id_low:   Lower  32 bits of session ID
  randssd:              56-bit random number from the base station for SSD
                                generation.
  process_control:  Process control bits
  esn:                     Electronic Serial Number of the ME

  ------------------------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* randssd_high */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->randssd_high);

  /* randssd_low  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->randssd_low);

  /* process_control */
  uimdiag_mmgsdi_util_decode_uint8((void*)&cmd_ptr->process_control);

  /* esn */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->esn);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_ssd_update */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_SSD_CONFIRM

DESCRIPTION
  This function parses cmd_ptr and is used to get all information
  related to UIMDIAG_MMGSDI_SESSION_SSD_CONFIRM

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_ssd_confirm(
    uimdiag_mmgsdi_session_confirm_ssd_cmd_type        * cmd_ptr,
    const void                                         * diag_req_ptr,
    uint32                                               diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);

  /* ------------------------------------------------------------------
  Parse following fields from diag buffer

  session_id_high:   Higher 32 bits of session ID
  session_id_low:    Lower  32 bits of session ID
  authbs:                 Authentication signature from the base station.
  ------------------------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* authbs */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->authbs);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_ssd_confirm */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_BS_CHAL

DESCRIPTION
  This function parses cmd_ptr and is used to get all information
  related to UIMDIAG_MMGSDI_SESSION_BS_CHAL

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_bs_chal(
    uimdiag_mmgsdi_session_bs_chal_cmd_type            * cmd_ptr,
    const void                                         * diag_req_ptr,
    uint32                                               diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);

 /* ------------------------------------------------------------------
  Parse following fields from diag buffer

  session_id_high:   Higher 32 bits of session ID
  session_id_low:    Lower  32 bits of session ID
  randseed:             Random number generator seed.
  ------------------------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* randseed */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->randseed);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_bs_chal */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_GET_INFO_SYNC

DESCRIPTION
  This function parses cmd_ptr and is used to get all information
  related to UIMDIAG_MMGSDI_SESSION_GET_INFO_SYNC

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_get_info_sync(
    uimdiag_mmgsdi_session_get_info_sync_cmd_type      * cmd_ptr,
    const void                                         * diag_req_ptr,
    uint32                                               diag_req_len)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* ------------------------------------------------------------------
   query_type:          0 - By Session_id, 1 - By session type
   session_id_high:   Higher 32 bits of session ID
   session_id_low:    Lower  32 bits of session ID
   session_type:       Session Type
   --------------------------------------------------------------------*/
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);
   /* query_type */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->query_type);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* session_type */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_type);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_get_info_sync */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_util_populate_read_cache_file_size_cmd

DESCRIPTION
  Gets the size of the file to be cached for a read operation/Read the cached file.
DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_read_cache_file_size_cmd(
    uimdiag_mmgsdi_session_read_cache_file_size_cmd_type  * cmd_ptr,
    const void                                         * diag_req_ptr,
    uint32                                               diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);

  /* ------------------------------------------------------------------
  Parse following fields from diag buffer

  session_id_high:   Higher 32 bits of session ID
  session_id_low:    Lower  32 bits of session ID
  file_name:            EF
  ------------------------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* file_name */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->file_name);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_read_cache_file_size_cmd */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_util_populate_read_cache_cmd

DESCRIPTION
  Gets the size of the file to be cached for a read operation/Read the cached file.
DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_read_cache_cmd(
    uimdiag_mmgsdi_session_read_cache_cmd_type         * cmd_ptr,
    const void                                         * diag_req_ptr,
    uint32                                               diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);

  /* ------------------------------------------------------------------
  Parse following fields from diag buffer

  session_id_high:   Higher 32 bits of session ID
  session_id_low:    Lower  32 bits of session ID
  file_name:            EF
  ------------------------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* file_name */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->file_name);

  /* offset updated to get data length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->data_len);

  /* offset updated to access the data Pointer */

  if(cmd_ptr->data_len == 0 || cmd_ptr->data_len > UIMDIAG_MAX_READ_DATA_LEN)
  {
    return UIMDIAG_ERROR;
  }
  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_read_cache_cmd */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_util_populate_write_cache

DESCRIPTION
  This function parses cmd_ptr and is used to get all information
  related to UIMDIAG_MMGSDI_SESSION_WRITE_CACHE

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_write_cache(
    uimdiag_mmgsdi_session_write_cache_cmd_type            * cmd_ptr,
    const void                                         * diag_req_ptr,
    uint32                                               diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);

  /* ------------------------------------------------------------------
  Parse following fields from diag buffer

  session_id_high:   Higher 32 bits of session ID
  session_id_low:    Lower  32 bits of session ID
  file_name:             Random number generator seed.
  data_len:              Length of data to write
  data:                    Data to write
  ------------------------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* file_name */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->file_name);

  /* data_len */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->data_len);

  /* data */
  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->data),
     cmd_ptr->data_len);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_write_cache */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_util_populate_pwr_ctrl_sync

DESCRIPTION
  This function parses cmd_ptr and is used to get all information
  related to UIMDIAG_MMGSDI_SESSION_TOGGLE_UIM_PWR_CTRL_SYNC

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_pwr_ctrl_sync(
    uimdiag_mmgsdi_session_toggle_uim_pwr_ctrl_cmd_type * cmd_ptr,
    const void                                          * diag_req_ptr,
    uint32                                                diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);

  /* ------------------------------------------------------------------
  Parse following fields from diag buffer

  session_id_high:   Higher 32 bits of session ID
  session_id_low:    Lower  32 bits of session ID
  in_TC:             Indicates whether the modem is in or out of the traffic channel.
  mode:              Mode on which traffic channel established GW or 1X
  ------------------------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* in_TC */
  uimdiag_mmgsdi_util_decode_uint8((void*)&cmd_ptr->in_TC);

  /* mode */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->mode);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_pwr_ctrl_sync */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_util_populate_sap_get_atr

DESCRIPTION
  This function parses cmd_ptr and is used to get all information
  related to UIMDIAG_MMGSDI_SAP_GET_ATR

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_sap_get_atr(
    uimdiag_mmgsdi_sap_get_atr_cmd_type * cmd_ptr,
    const void                          * diag_req_ptr,
    uint32                                diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);

  /* ------------------------------------------------------------------
  Parse following fields from diag buffer

  client_id_high:   Higher 32 bits of client  ID
  client_id_low:    Lower  32 bits of client  ID
  card_slot:        Indicates the SLOT

  ------------------------------------------------------------------- */

  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_high);

  /* offset updated to access Client ID Lower 32 bits */

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_low);

  /* Card slot  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->card_slot);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_sap_get_atr */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_util_populate_sap_send_apdu

DESCRIPTION
  This function parses cmd_ptr and is used to get all information
  related to UIMDIAG_MMGSDI_SAP_SEND_APDU

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_sap_send_apdu(
    uimdiag_mmgsdi_sap_send_apdu_cmd_type * cmd_ptr,
    const void                            * diag_req_ptr,
    uint32                                  diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);

  /* ------------------------------------------------------------------
  Parse following fields from diag buffer

  client_id_high:   Higher 32 bits of client  ID
  client_id_low:    Lower  32 bits of client  ID
  card_slot:        Indicates the SLOT
  channel_id:       The logical channel id
  apdu_data_len:    Length of the APDU
  apdu_data_ptr:    Holds Apdu data
  ------------------------------------------------------------------- */

  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_high);

  /* offset updated to access Client ID Lower 32 bits */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_low);

  /* Card slot  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->card_slot);

  /* Channel Id */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->channel_id);

  /* APDU Data length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->apdu_data_len);

  /* APDU data */
  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->apdu_data_ptr),
     cmd_ptr->apdu_data_len);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_sap_send_apdu */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_util_populate_sap_generic

DESCRIPTION
  This function parses cmd_ptr and is used to get all information
  related to UIMDIAG_MMGSDI_SAP_POWER_ON
             UIMDIAG_MMGSDI_SAP_POWER_OFF
             UIMDIAG_MMGSDI_UTIL_POPULATE_SAP_CARD_READER_STATUS
             UIMDIAG_MMGSDI_SAP_RESET


DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_sap_generic(
    uimdiag_mmgsdi_sap_generic_cmd_type * cmd_ptr,
    const void                           * diag_req_ptr,
    uint32                                 diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);

  /* ------------------------------------------------------------------
  Parse following fields from diag buffer

  client_id_high:   Higher 32 bits of client  ID
  client_id_low:    Lower  32 bits of client  ID
  card_slot:        Indicates the SLOT

  ------------------------------------------------------------------- */

  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_high);

  /* offset updated to access Client ID Lower 32 bits */

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_low);

  /* Card slot  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->card_slot);

  /* SAP mode */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->sap_mode);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_sap_generic */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_util_populate_card_pdown

DESCRIPTION
  This function parses cmd_ptr and is used to get all information
  related to UIMDIAG_MMGSDI_CARD_PDOWN

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_card_pdown(
    uimdiag_mmgsdi_card_pdown_cmd_type * cmd_ptr,
    const void                         * diag_req_ptr,
    uint32                               diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* ------------------------------------------------------------------
  Parse following fields from diag buffer

  client_id_high:   Higher 32 bits of client  ID
  client_id_low:    Lower  32 bits of client  ID
  slot_id:          Indicates the SLOT
  power_down_option:Indicates whether to just power down the CARD or
                    to do a power down and notify GSDI about card
                    removal

  ------------------------------------------------------------------- */

  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_high);

  /* offset updated to access Client ID Lower 32 bits */

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_low);

  /* Card slot  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->slot_id);

  /* power_down_option */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->power_down_option);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_card_pdown */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_util_populate_card_pup

DESCRIPTION
  This function parses cmd_ptr and is used to get all information
  related to UIMDIAG_MMGSDI_CARD_PUP

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_card_pup(
    uimdiag_mmgsdi_card_pup_cmd_type * cmd_ptr,
    const void                       * diag_req_ptr,
    uint32                             diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* ------------------------------------------------------------------
  Parse following fields from diag buffer

  client_id_high:   Higher 32 bits of client  ID
  client_id_low:    Lower  32 bits of client  ID
  slot_id:          Indicates the SLOT
  power_up_option:  Card power up option

  ------------------------------------------------------------------- */

  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_high);

  /* offset updated to access Client ID Lower 32 bits */

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_low);

  /* Card slot  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->slot_id);

  /* power_up_option */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->power_up_option);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_card_pup */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_util_populate_activate_onchip_sim

DESCRIPTION
  This function parses cmd_ptr and is used to get all information
  related to UIMDIAG_MMGSDI_ACTIVATE_ONCHIP_SIM

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_activate_onchip_sim(
    uimdiag_mmgsdi_activate_onchip_sim_cmd_type * cmd_ptr,
    const void                                  * diag_req_ptr,
    uint32                                        diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* ------------------------------------------------------------------
  Parse following fields from diag buffer

  client_id_high:                      Higher 32 bits of client  ID
  client_id_low:                       Lower  32 bits of client  ID
  slot_id:                             Indicates the SLOT
  onchip_data_length, onchip_data_ptr: TLV data length and data

  ------------------------------------------------------------------- */

  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_high);

  /* offset updated to access Client ID Lower 32 bits */

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_low);

  /* Card slot  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->slot_id);

  /* offset updated to get TLV data length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->onchip_data_length);

  /* offset updated to access the TLV data pointer */
  if(cmd_ptr->onchip_data_length == 0)
  {
    cmd_ptr->onchip_data_ptr = NULL;
    return UIMDIAG_ERROR;
  }

  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->onchip_data_ptr),
    cmd_ptr->onchip_data_length);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_activate_onchip_sim */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_util_populate_send_apdu

DESCRIPTION
  This function parses cmd_ptr and is used to get all information
  related to UIMDIAG_MMGSDI_SEND_APDU

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_send_apdu(
    uimdiag_mmgsdi_send_apdu_cmd_type * cmd_ptr,
    const void                        * diag_req_ptr,
    uint32                              diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* ------------------------------------------------------------------
  Parse following fields from diag buffer

  channel_id:       Default channel ID
  slot_id:          Indicates the SLOT
  apdu_data_length: APDU buffer length
  apdu_data_ptr:    APDU buffer

  ------------------------------------------------------------------- */

  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_high);

  /* offset updated to access Client ID Lower 32 bits */

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_low);

  /* Card slot  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->slot_id);

   /* APDU Data length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->apdu_data_length);

  /* APDU data */
  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->apdu_data_ptr),
     cmd_ptr->apdu_data_length);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_send_apdu */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_util_populate_sap_disconnect

DESCRIPTION
  This function parses cmd_ptr and is used to get all information
  related to UIMDIAG_MMGSDI_SAP_DISCONNECT

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_sap_disconnect(
    uimdiag_mmgsdi_sap_disconnect_cmd_type * cmd_ptr,
    const void                             * diag_req_ptr,
    uint32                                   diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);

  /* ------------------------------------------------------------------
  Parse following fields from diag buffer

  client_id_high:   Higher 32 bits of client  ID
  client_id_low:    Lower  32 bits of client  ID
  card_slot:        Indicates the SLOT

  ------------------------------------------------------------------- */

  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_high);

  /* offset updated to access Client ID Lower 32 bits */

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_low);

  /* Card slot  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->card_slot);

  /* SAP mode */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->sap_mode);

  /* SAP mode */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->sap_disconnect_mode);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_sap_generic */

/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_GET_OPERATOR_NAME_CMD

DESCRIPTION
  This command is used to build a request to MMGSDI for get operator name.

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_session_get_operator_name_cmd(
  uimdiag_mmgsdi_session_get_operator_name_cmd_type    * cmd_ptr,
  const void                                           * diag_req_ptr,
  uint32                                                 diag_req_len)
  {

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1.Session Id Lower/Higher 32 bits
  2. No of PLMNs
  3. PLMN ID(s)
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to access "no_of_plmns" */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->no_of_plmns);

  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->plmn_list_ptr),
    cmd_ptr->no_of_plmns * 4); /*4 is UIMDIAG input array length for PLMN-RAT*/

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_get_operator_name_cmd */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_util_populate_get_all_available_app_cmd

DESCRIPTION
  Gets the client id for get all available apps sync cmd.
DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_get_all_available_app_cmd(
    uimdiag_mmgsdi_get_all_available_apps_cmd_type     * cmd_ptr,
    const void                                         * diag_req_ptr,
    uint32                                               diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* ------------------------------------------------------------------
  Parse following fields from diag buffer

  client_id_high:   Higher 32 bits of client ID
  client_id_low:    Lower  32 bits of client ID
  ------------------------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);


  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_high);

  /* offset updated to access Client ID Lower 32 bits */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_low);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_get_all_available_app_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_OPEN_EXT_CMD

DESCRIPTION
  This function parses cmd_ptr and fills the session open ext command params

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_session_open_ext_cmd(
  uimdiag_mmgsdi_session_open_ext_cmd_type         * cmd_ptr,
  const void                                       * diag_req_ptr,
  uint32                                             diag_req_len)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1. Client Id Lower/Higher 32 bits
  2. Session Type_mask
  3. Set Notify_mask

  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* offset updated to access Client ID Higher 32 bits */

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_high);

  /* offset updated to access Client ID Lower 32 bits */

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_low);

  /* offset updated to access the session type mask field*/

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_type_mask);

  /* offset updated to access the set notify mask field */

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->set_notify_mask);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_open_ext_cmd */
/* ==========================================================================
   FUNCTION:      UIMDIAG_UTIL_CONVERT_TO_MMGSDI_ITEMS_ENUM

   DESCRIPTION:
     This function checks for valid range of the mmgsdi_file_enum_type and
     returns the corresponding mmgsdi_enum_type

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_NO_FILE_ENUM     : uim item not found in the conversion table


   SIDE EFFECTS:
     None
==========================================================================*/
mmgsdi_file_enum_type uimdiag_util_convert_to_mmgsdi_items_enum (
  uimdiag_file_enum_type uimdiag_mmgsdi_file
)
{
  uint32 i     = 0;
  uint32 count = (uint32)(sizeof(uimdiag_file_to_mmgsdi_table) /
                          sizeof(uimdiag_file_to_mmgsdi_map));

  for (i = 0; i< count; i++)
  {
    if (uimdiag_file_to_mmgsdi_table[i].uimdiag_file == uimdiag_mmgsdi_file)
    {
      return uimdiag_file_to_mmgsdi_table[i].mmgsdi_file;
    }
  }
  return MMGSDI_NO_FILE_ENUM;
} /* uimdiag_util_convert_to_mmgsdi_items_enum */

/* ==========================================================================
   FUNCTION:      UIMDIAG_UTIL_CONVERT_FROM_MMGSDI_ITEMS_ENUM

   DESCRIPTION:
     This function checks for valid range of the mmgsdi_file_enum_type and
     returns the corresponding uimdiag_file_enum_type

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     uimdiag_file_enum_type


   SIDE EFFECTS:
     None
==========================================================================*/
uimdiag_file_enum_type uimdiag_util_convert_from_mmgsdi_items_enum (
  mmgsdi_file_enum_type mmgsdi_file
)
{
  uint32 i     = 0;
  uint32 count = (uint32)(sizeof(uimdiag_file_to_mmgsdi_table) /
                          sizeof(uimdiag_file_to_mmgsdi_map));

  for (i = 0; i< count; i++)
  {
    if (uimdiag_file_to_mmgsdi_table[i].mmgsdi_file == mmgsdi_file)
    {
      return uimdiag_file_to_mmgsdi_table[i].uimdiag_file;
    }
  }
  return UIMDIAG_NO_FILE_ENUM;
} /* uimdiag_util_convert_from_mmgsdi_items_enum */
#endif /* FEATURE_UIMDIAG */
