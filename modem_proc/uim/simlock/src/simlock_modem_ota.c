/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    S I M L O C K   M O D E M   O T A   F U N C T I O N S


GENERAL DESCRIPTION

  This file contains the SIMLOCK OTA unlock and supporting functions.

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2014 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock_modem_ota.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
09/07/16   vdc     Use correct IMEI in OTA unlock and RSU blob validation
05/22/16   vdc     Remove F3 messages for memory allocation failure
05/15/15   vv      Support for RSU set configuration in simlock
07/08/14   vv      Fix for a compiler warning
06/03/14   vv      Initial revision
=============================================================================*/

/*=============================================================================

                         INCLUDE FILES FOR MODULE

=============================================================================*/
#include "uim_variation.h"
#include "customer.h"
#include "uim_msg.h"

#ifdef FEATURE_SIMLOCK

#include "simlock_modem_ota.h"
#include "simlock_modem.h"
#include "simlocklib.h"
#include "simlock_platform.h"
#include "simlock_util.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* As per the 3GPP spec. 22.022, Annex A.4, the
   indexes in OTA unlock request and response are
   defined as below */

/* These are the corresponding indexes in the
   OTA request data */
#define SIMLOCK_OTA_IMEI_INDEX              72

#define SIMLOCK_OTA_UNLOCK_NW_CK_INDEX      40
#define SIMLOCK_OTA_UNLOCK_NS_CK_INDEX      48
#define SIMLOCK_OTA_UNLOCK_SP_CK_INDEX      56
#define SIMLOCK_OTA_UNLOCK_CP_CK_INDEX      64

#define SIMLOCK_OTA_UNLOCK_NW_CK_INDEX_EXT  88
#define SIMLOCK_OTA_UNLOCK_NS_CK_INDEX_EXT  96
#define SIMLOCK_OTA_UNLOCK_SP_CK_INDEX_EXT  104
#define SIMLOCK_OTA_UNLOCK_CP_CK_INDEX_EXT  112

/* The maximum length of CK in OTA request
   either in main or ext data */
#define SIMLOCK_OTA_UNLOCK_CK_MAX           8

/* These are the corresponding indexes in the
   OTA response data */
#define SIMLOCK_OTA_RSP_NW_INDEX            16
#define SIMLOCK_OTA_RSP_NS_INDEX            17
#define SIMLOCK_OTA_RSP_SP_INDEX            18
#define SIMLOCK_OTA_RSP_CP_INDEX            19

/* As per the 3GPP spec. 22.022, Annex A.4, the
   the status for a category in OTA unlock
   response data can be one of the following */
#define SIMLOCK_CURRENTLY_NOT_LOCKED        0
#define SIMLOCK_PERMANENTLY_NOT_LOCKED      1
#define SIMLOCK_CURRENTLY_LOCKED            2
#define SIMLOCK_IMEI_MISMATCH               3

/*=============================================================================

                                  FUNCTIONS

=============================================================================*/

/*===========================================================================
FUNCTION SIMLOCK_MODEM_OTA_GET_IMEI

DESCRIPTION 
  This function extracts the IMEI from the OTA request message

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS 
  NONE 
===========================================================================*/
static simlock_result_enum_type simlock_modem_ota_get_imei(
  const simlock_ota_unlock_data_type    * ota_unlock_data_ptr,
  simlock_imei_type                     * ota_imei_data_ptr
)
{
  if((ota_unlock_data_ptr == NULL) ||
     (ota_imei_data_ptr == NULL))
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  if(ota_unlock_data_ptr->ota_data_len >=
                (SIMLOCK_OTA_IMEI_INDEX + SIMLOCK_IMEI_MAX))
  {
    simlock_memscpy(ota_imei_data_ptr->imei_data,
                    SIMLOCK_IMEI_MAX,
                    &ota_unlock_data_ptr->ota_data[SIMLOCK_OTA_IMEI_INDEX],
                    SIMLOCK_IMEI_MAX);
    ota_imei_data_ptr->imei_len = SIMLOCK_IMEI_MAX;
    return SIMLOCK_SUCCESS;
  }

  return SIMLOCK_GENERIC_ERROR;
} /* simlock_modem_ota_get_imei */


/*===========================================================================
FUNCTION SIMLOCK_MODEM_GET_OTA_CK

DESCRIPTION
  This functions extracts the CK from the OTA input data

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_modem_ota_get_ck(
  const simlock_ota_unlock_data_type      * unlock_data_ptr,
  simlock_category_enum_type                category,
  simlock_ck_data_type                    * ck_data_ptr
)
{
  uint8   ck_index     = 0;
  uint8   ck_index_ext = 0;
  uint8   index        = 0;
  uint8   index_ext    = 0;

  if((unlock_data_ptr == NULL) ||
     (ck_data_ptr == NULL))
  {
    UIM_MSG_ERR_0("invalid input parameters");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  switch(category)
  {
    case SIMLOCK_CATEGORY_3GPP_NW:
      ck_index     = SIMLOCK_OTA_UNLOCK_NW_CK_INDEX;
      ck_index_ext = SIMLOCK_OTA_UNLOCK_NW_CK_INDEX_EXT;
      break;

    case SIMLOCK_CATEGORY_3GPP_NS:
      ck_index = SIMLOCK_OTA_UNLOCK_NS_CK_INDEX;
      ck_index_ext = SIMLOCK_OTA_UNLOCK_NS_CK_INDEX_EXT;
      break;

    case SIMLOCK_CATEGORY_3GPP_SP:
      ck_index = SIMLOCK_OTA_UNLOCK_SP_CK_INDEX;
      ck_index_ext = SIMLOCK_OTA_UNLOCK_SP_CK_INDEX_EXT;
      break;

    case SIMLOCK_CATEGORY_3GPP_CP:
      ck_index = SIMLOCK_OTA_UNLOCK_CP_CK_INDEX;
      ck_index_ext = SIMLOCK_OTA_UNLOCK_CP_CK_INDEX_EXT;
      break;

    default:
      return SIMLOCK_GENERIC_ERROR;
  }

  /* extract the CK from main data in OTA */
  for(index = 0; index < SIMLOCK_OTA_UNLOCK_CK_MAX;
      index++)
  {
    if((ck_index + index) >= unlock_data_ptr->ota_data_len)
    {
      return SIMLOCK_GENERIC_ERROR;
    }

    /* as per the 3GPP spec, 22.022 Annex A4, 'F' is used
       for padding the key */
    /* if 'F' is found, it indicates end of the CK */
    if(unlock_data_ptr->ota_data[ck_index + index] == 'F')
    {
      break;
    }

    if(index < SIMLOCK_CK_MAX)
    {
      ck_data_ptr->simlock_ck_data[index] =
             (char)unlock_data_ptr->ota_data[ck_index + index];
      ck_data_ptr->simlock_ck_length++;
    }
  }

  /* extract the CK from ext data, if present */
  if((unlock_data_ptr->ota_data_len  == SIMLOCK_OTA_UNLOCK_DATA_MAX) &&
     (ck_data_ptr->simlock_ck_length == SIMLOCK_OTA_UNLOCK_CK_MAX))
  {
    for(index_ext = 0; index_ext < SIMLOCK_OTA_UNLOCK_CK_MAX;
        index_ext++)
    {
      if((ck_index_ext + index_ext) >= unlock_data_ptr->ota_data_len)
      {
        return SIMLOCK_GENERIC_ERROR;
      }

      /* as per the 3GPP spec, 22.022 Annex A4, 'F' is used
         for padding the key */
      /* if 'F' is found, it indicates end of the CK */
      if(unlock_data_ptr->ota_data[ck_index_ext + index_ext] == 'F')
      {
        break;
      }

      if(index < SIMLOCK_CK_MAX)
      {
        ck_data_ptr->simlock_ck_data[index++] =
               (char)unlock_data_ptr->ota_data[ck_index_ext + index_ext];
        ck_data_ptr->simlock_ck_length++;
      }
    }
  }

  return SIMLOCK_SUCCESS;
} /* simlock_modem_ota_get_ck */


/*===========================================================================
FUNCTION SIMLOCK_MODEM_OTA_CHECK_LOCK_STATUS

DESCRIPTION
  This function sends the get status request to the SIMLOCK engine and
  checks the lock status of the requested category

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_modem_ota_check_lock_status(
  simlock_slot_enum_type        slot,
  simlock_category_enum_type    category,
  boolean                     * lock_status_ptr
)
{
  simlock_result_enum_type              simlock_status      = SIMLOCK_SUCCESS;
  simlock_message_request_data_type   * get_status_req_ptr  = NULL;
  simlock_message_response_data_type  * get_status_resp_ptr = NULL;
  uint8                                 slot_index          = 0;
  uint8                                 category_index      = 0;

  if(lock_status_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  SIMLOCK_CHECK_AND_MEM_MALLOC(get_status_req_ptr,
                               sizeof(simlock_message_request_data_type));
  if (get_status_req_ptr == NULL)
  {
    return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  SIMLOCK_CHECK_AND_MEM_MALLOC(get_status_resp_ptr,
                               sizeof(simlock_message_response_data_type));
  if (get_status_resp_ptr == NULL)
  {
    SIMLOCK_MEM_FREE(get_status_req_ptr);
    return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  get_status_req_ptr->msg_type = SIMLOCK_GET_STATUS_MSG;

  /* call the engine with get status request */
  simlock_status = simlock_process_message(get_status_req_ptr, get_status_resp_ptr);
  if(simlock_status == SIMLOCK_SUCCESS)
  {
    simlock_status = simlock_util_get_slot_index(slot, &slot_index);
    if((simlock_status == SIMLOCK_SUCCESS) &&
       (slot_index < SIMLOCK_SLOT_COUNT_MAX))
    {
      simlock_status = simlock_util_get_category_index(category, &category_index);
      if((simlock_status == SIMLOCK_SUCCESS) &&
         (category_index < SIMLOCK_CATEGORY_COUNT_MAX))
      {
        *lock_status_ptr =
            get_status_resp_ptr->message.get_status_resp.lock_info[slot_index].category_info[category_index].category_status;
      }
    }
  }

  SIMLOCK_MEM_FREE(get_status_req_ptr);
  SIMLOCK_MEM_FREE(get_status_resp_ptr);

  return simlock_status;
} /* simlock_modem_ota_check_lock_status */


/*===========================================================================
FUNCTION SIMLOCK_MODEM_OTA_SEND_UNLOCK

DESCRIPTION
  This function builds the unlock request for a category and sends the request
  to the SIMLOCK engine.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_modem_ota_send_unlock(
  const simlock_ck_data_type  * ck_data_ptr,
  simlock_slot_enum_type        slot,
  simlock_category_enum_type    category
)
{
  simlock_result_enum_type              simlock_status  = SIMLOCK_SUCCESS;
  simlock_message_request_data_type   * unlock_req_ptr  = NULL;
  simlock_message_response_data_type  * unlock_resp_ptr = NULL;

  if((ck_data_ptr == NULL) ||
     (ck_data_ptr->simlock_ck_length > SIMLOCK_CK_MAX))
  {
    UIM_MSG_ERR_0("invalid input parameters");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  SIMLOCK_CHECK_AND_MEM_MALLOC(unlock_req_ptr,
                               sizeof(simlock_message_request_data_type));
  if (unlock_req_ptr == NULL)
  {
    return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  SIMLOCK_CHECK_AND_MEM_MALLOC(unlock_resp_ptr,
                               sizeof(simlock_message_response_data_type));
  if (unlock_req_ptr == NULL)
  {
    SIMLOCK_MEM_FREE(unlock_req_ptr);
    return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  unlock_req_ptr->msg_type                    = SIMLOCK_UNLOCK_DEVICE_MSG;
  unlock_req_ptr->message.unlock_msg.slot     = slot;
  unlock_req_ptr->message.unlock_msg.category = category;
  unlock_req_ptr->message.unlock_msg.ck.simlock_ck_length =
     ck_data_ptr->simlock_ck_length;

  simlock_memscpy(unlock_req_ptr->message.unlock_msg.ck.simlock_ck_data,
                  SIMLOCK_CK_MAX,
                  ck_data_ptr->simlock_ck_data,
                  ck_data_ptr->simlock_ck_length);

  /* call the engine with unlock request */
  simlock_status = simlock_process_message(unlock_req_ptr, unlock_resp_ptr);

  SIMLOCK_MEM_FREE(unlock_req_ptr);
  SIMLOCK_MEM_FREE(unlock_resp_ptr);

  return simlock_status;
} /* simlock_modem_ota_send_unlock */


/*===========================================================================
FUNCTION SIMLOCK_MODEM_PROCESS_OTA_UNLOCK

DESCRIPTION
  This function process the OTA unlock request. It first gets the IMEI of
  the device and compares with that in the OTA request. If they match,
  then it retrieves the CK from the request data and sends an unlock
  request to the SIMLOCK engine

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_modem_ota_process_unlock(
  const simlock_ota_unlock_data_type * ota_unlock_data_ptr,
  simlock_slot_enum_type               slot,
  simlock_ota_unlock_resp_type         unlock_resp_data
)
{
  simlock_result_enum_type      simlock_status = SIMLOCK_SUCCESS;
  uint8                         category_index = SIMLOCK_OTA_RSP_NW_INDEX;
  simlock_category_enum_type    category       = SIMLOCK_CATEGORY_3GPP_NW;
  boolean                       lock_status    = FALSE;
  simlock_imei_type             ota_imei;
  simlock_imei_type             device_imei;

  if(ota_unlock_data_ptr == NULL)
  {
    UIM_MSG_ERR_0("invalid input parameters");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  memset(&ota_imei, 0x00, sizeof(simlock_imei_type));
  memset(&device_imei, 0x00, sizeof(simlock_imei_type));

  /* get the ota imei */
  simlock_status = simlock_modem_ota_get_imei(ota_unlock_data_ptr, &ota_imei);
  if((simlock_status != SIMLOCK_SUCCESS) ||
     (ota_imei.imei_len == 0))
  {
    UIM_MSG_ERR_0("invalid ota imei");
    return SIMLOCK_GENERIC_ERROR;
  }

  /* get the device imei */
  simlock_status = simlock_util_get_imei(slot,
                                         &device_imei);
  if((simlock_status != SIMLOCK_SUCCESS) ||
     (device_imei.imei_len == 0))
  {
    UIM_MSG_ERR_0("invalid device imei");
    return SIMLOCK_GENERIC_ERROR;
  }

  /* copy the device imei to the response */
  simlock_memscpy(unlock_resp_data,
                  SIMLOCK_IMEI_MAX,
                  device_imei.imei_data,
                  device_imei.imei_len);

  /* as per the 3GPP spec, 22.022 Annex A4, the
     16 digit is set to 0 */
  /* set the 16 byte in response to 0 */
  unlock_resp_data[SIMLOCK_IMEI_MAX] = '0';

  /* check if the IMEIs match */
  if(!simlock_util_check_imei_matches(device_imei, ota_imei))
  {
    UIM_MSG_ERR_0("imei mismatch");
    unlock_resp_data[SIMLOCK_OTA_RSP_NW_INDEX] = SIMLOCK_IMEI_MISMATCH + '0';
    unlock_resp_data[SIMLOCK_OTA_RSP_NS_INDEX] = SIMLOCK_IMEI_MISMATCH + '0';
    unlock_resp_data[SIMLOCK_OTA_RSP_SP_INDEX] = SIMLOCK_IMEI_MISMATCH + '0';
    unlock_resp_data[SIMLOCK_OTA_RSP_CP_INDEX] = SIMLOCK_IMEI_MISMATCH + '0';
    return SIMLOCK_SUCCESS;
  }

  for(category = SIMLOCK_CATEGORY_3GPP_NW;
      category <= SIMLOCK_CATEGORY_3GPP_CP;
      category++)
  {
    simlock_ck_data_type          ck_data;
    memset(&ck_data, 0x00, sizeof(simlock_ck_data_type));

    switch(category)
    {
      case SIMLOCK_CATEGORY_3GPP_NW:
        category_index = SIMLOCK_OTA_RSP_NW_INDEX;
        break;

      case SIMLOCK_CATEGORY_3GPP_NS:
        category_index = SIMLOCK_OTA_RSP_NS_INDEX;
        break;

      case SIMLOCK_CATEGORY_3GPP_SP:
        category_index = SIMLOCK_OTA_RSP_SP_INDEX;
        break;

      case SIMLOCK_CATEGORY_3GPP_CP:
        category_index = SIMLOCK_OTA_RSP_CP_INDEX;
        break;

      default:
        break;
    }

    /* get the ck from ota data */
    simlock_status = simlock_modem_ota_get_ck(ota_unlock_data_ptr,
                                              category,
                                              &ck_data);
    if(simlock_status != SIMLOCK_SUCCESS)
    {
      UIM_MSG_ERR_1("unable to extract CK for category, 0x%x", category);
      unlock_resp_data[category_index] = SIMLOCK_CURRENTLY_NOT_LOCKED + '0';
      return SIMLOCK_GENERIC_ERROR;
    }

    if(ck_data.simlock_ck_length == 0)
    {
      /* In this case, as per the spec 22.002, the unlock
         of the category is not required. So need to query
         the status and indicate it in the response */
      simlock_status = simlock_modem_ota_check_lock_status(slot, category,
                                                           &lock_status);
      if(simlock_status != SIMLOCK_SUCCESS)
      {
        UIM_MSG_ERR_1("unable to check lock status for category, 0x%x", category);
        return SIMLOCK_GENERIC_ERROR;
      }
      if(lock_status)
      {
        unlock_resp_data[category_index] = SIMLOCK_CURRENTLY_LOCKED + '0';
      }
      else
      {
        unlock_resp_data[category_index] = SIMLOCK_CURRENTLY_NOT_LOCKED + '0';
      }
      continue;
    }

    /* call the engine for unlock */
    simlock_status = simlock_modem_ota_send_unlock(&ck_data, slot,
                                                   category);
    if((simlock_status == SIMLOCK_SUCCESS) ||
       (simlock_status == SIMLOCK_INVALID_STATE))
    {
      unlock_resp_data[category_index] = SIMLOCK_CURRENTLY_NOT_LOCKED + '0';
    }
    else
    {
      unlock_resp_data[category_index] = SIMLOCK_CURRENTLY_LOCKED + '0';
    }
  }

  return SIMLOCK_SUCCESS;
} /* simlock_modem_ota_process_unlock */

#endif /* FEATURE_SIMLOCK */

