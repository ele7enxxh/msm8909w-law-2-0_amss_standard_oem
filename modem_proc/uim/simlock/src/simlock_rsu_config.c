/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            S I M   L O C K   R S U   C O N F I G   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the simlock RSU configuration functions.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2015 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock_rsu_config.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/08/16   bcho    Added support for ICCID simlock
09/07/16   vdc     Use correct IMEI in OTA unlock and RSU blob validation
08/04/16   vdc     Fix compiler errors for FEATURE_SIMLOCK_QC_REMOTE_LOCK
06/17/16   vdc     Added support for SPN based SIMLOCK feature
01/20/16   stv     Fix to validate the RSA signature in the blob request
01/07/16   stv     Move variables to heap to reduce stack usage
01/06/16   stv     Remote simlock support
08/19/15   stv     Restrict native simlock locking when rsu is enabled
08/12/15   stv     Clear temp unlock data when temp unlock expires
08/10/15   stv     To send temporary unlock status indication
08/04/15   stv     Allow the rsu set configuration with/without protection in test environment 
07/06/15   stv     Clear temp unlock data when a permanent unlock/reset simlock req
06/08/15   stv     Featurize integrity checks for rsu set configuration cmd
05/17/15   stv     Fix compiler warnings
05/15/15   vv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "customer.h"

#if defined (FEATURE_SIMLOCK) && (defined (FEATURE_SIMLOCK_RSU) || defined (FEATURE_SIMLOCK_QC_REMOTE_LOCK))

#include "simlock.h"
#include "simlock_modem.h"
#include "simlock_crypto.h"
#include "simlock_rsu_config.h"
#include "simlock_rsu_key.h"
#include "simlock_temp_unlock.h"
#include "simlock_time.h"
#include "simlock_util.h"
#include "simlocklib.h"

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#define SIMLOCK_RSU_SWAP_BYTE_ORDER_16(x)      \
     (((x >> 8) & 0xFF) + ((x & 0xFF) << 8))

#define SIMLOCK_RSU_SWAP_BYTE_ORDER_32(x)      \
     (((x & 0xFF000000) >> 24) + ((x & 0x00FF0000) >> 8) + ((x & 0x0000FF00) << 8) + ((x & 0x000000FF) << 24))

#define SIMLOCK_RSU_SWAP_BYTE_ORDER_64(x)      \
     (((x & 0xFF00000000000000) >> 56) + ((x & 0x00FF000000000000) >> 40) + ((x & 0x0000FF0000000000) >> 24) + ((x & 0x000000FF00000000) >> 8) +    \
      ((x & 0x00000000FF000000) << 8) + ((x & 0x0000000000FF0000) << 24) + ((x & 0x000000000000FF00) << 40) + ((x & 0x00000000000000FF) << 56))

#define SIMLOCK_RSU_CONFIG_HIGH_NIBBLE              0xF0
#define SIMLOCK_RSU_CONFIG_LOW_NIBBLE               0x0F

#define SIMLOCK_RSU_CONFIG_HIGH_BYTE                0xFF00
#define SIMLOCK_RSU_CONFIG_LOW_BYTE                 0x00FF

#define SIMLOCK_RSU_CONFIG_GID_SIZE                 2

#define SIMLOCK_REMOTE_LOCK_TIME_FILE_VERSION       1

/* ----------------------------------------------------------------------------
   ENUM:      SIMLOCK_RFU_CONFIG_PROTECTION_ENUM_TYPE

   DESCRIPTION:
     This enum specifies the protection scheme of the config data
-------------------------------------------------------------------------------*/
typedef enum {
  SIMLOCK_RSU_CONFIG_PROTECTION_NONE    = 0,
  SIMLOCK_RSU_CONFIG_PROTECTION_SHA256,
  SIMLOCK_RSU_CONFIG_PROTECTION_RSA
}simlock_rsu_config_protection_enum_type;

/* ----------------------------------------------------------------------------
   ENUM:      SIMLOCK_RFU_CONFIG_LOCK_STATE_ENUM_TYPE

   DESCRIPTION:
     This enum specifies the current lock state in the config data
-------------------------------------------------------------------------------*/
typedef enum {
  SIMLOCK_RSU_CONFIG_LOCK_STATE_PERMANENT_LOCK  = 0,
  SIMLOCK_RSU_CONFIG_LOCK_STATE_TEMPORARY_UNLOCK,
  SIMLOCK_RSU_CONFIG_LOCK_STATE_PERMANENT_UNLOCK,
  SIMLOCK_RSU_CONFIG_LOCK_STATE_PARTIAL_UNLOCK
}simlock_rsu_config_lock_state_enum_type;

/* ----------------------------------------------------------------------------
   DATA TYPE:      simlock_rsu_config_mcc_mnc_type
------------------------------------------------------------------------------*/
typedef uint32 simlock_rsu_config_mcc_mnc_type;

/* ----------------------------------------------------------------------------
   DATA TYPE:      simlock_rsu_config_gid_type
------------------------------------------------------------------------------*/
typedef uint16 simlock_rsu_config_gid_type;

/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/
/* ----------------------------------------------------------------------------
   STRUCTURE:      SIMLOCK_RSU_NW_CATEGORY_INFO_TYPE

   DESCRIPTION:
     This structure contains the list of MCC/MNCs

     nw_codes       : Number of nw_codes in the list
     nw_code_ptr    : Pointer to nw code data
-------------------------------------------------------------------------------*/
typedef struct
{
  uint16                        nw_codes;
  simlock_nw_code_data_type    *nw_code_ptr;
}simlock_rsu_nw_category_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      SIMLOCK_RSU_NS_CATEGORY_INFO_TYPE

   DESCRIPTION:
     This structure contains the NS lock info

     ns_codes       : Number of ns codes in the list
     ns_code_ptr    : Pointer to NS code data
-------------------------------------------------------------------------------*/
typedef struct
{
  uint16                        ns_codes;
  simlock_ns_code_data_type    *ns_code_ptr;
}simlock_rsu_ns_category_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      SIMLOCK_RSU_SP_CATEGORY_INFO_TYPE

   DESCRIPTION:
     This structure contains the SP lock info

     sp_codes       : Number of sp codes in the list
     sp_code_ptr    : Pointer to SP code data
-------------------------------------------------------------------------------*/
typedef struct
{
  uint16                        sp_codes;
  simlock_sp_code_data_type    *sp_code_ptr;
}simlock_rsu_sp_category_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      SIMLOCK_RSU_CONFIG_CODE_DATA_TYPE

   DESCRIPTION:
     This structure contains the configuration data

     nw_lock     : nw lock information
     ns_lock      : ns lock information
     sp_lock     : sp lock information
-------------------------------------------------------------------------------*/
typedef struct
{
  simlock_rsu_nw_category_info_type        nw_lock;
  simlock_rsu_ns_category_info_type        ns_lock;
  simlock_rsu_sp_category_info_type        sp_lock;
}simlock_rsu_config_lock_code_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      SIMLOCK_RSU_CONFIG_HEADER_DATA_TYPE

   DESCRIPTION:
     This structure contains the header data of the RSU configuration

     version_major       : Major version of config data
     version_minor       : Minor version of config data
     protection_type     : Type of protection (None, MAC or RSA Sig)
     lock_state          : State of the simlock
     time_stamp          : Time stamp at which the blob is created
-------------------------------------------------------------------------------*/
typedef struct
{
  uint16                                     version_major;
  uint16                                     version_minor;
  simlock_rsu_config_protection_enum_type    protection_type;
  simlock_rsu_config_lock_state_enum_type    lock_state;
  uint64                                     time_stamp;
}simlock_rsu_config_header_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      SIMLOCK_RSU_CONFIG_CODE_DATA_TYPE

   DESCRIPTION:
     This structure contains the configuration data of the RSU

     lock_code_data   : Contains the lock code information
     start_time       : Temporary unlock start time (in seconds)
     end_time         : Temporary unlock end time (in seconds)
-------------------------------------------------------------------------------*/
typedef struct
{
  simlock_rsu_config_lock_code_data_type     lock_code_data;
  simlock_time_type                          start_time;
  simlock_time_type                          end_time;
}simlock_rsu_config_code_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      SIMLOCK_RSU_CONFIG_AUTH_DATA_TYPE

   DESCRIPTION:
     This structure contains the authentication parameters for the configuration
     data

     msg_len          : Length of the message
     msg_data_ptr     : Pointer to the message to be authenticated
     hmac             : The HMAC associated with the blob
-------------------------------------------------------------------------------*/
typedef struct
{
  uint16                                  msg_len;
  uint8                                  *msg_data_ptr;
  union {
    simlock_hmac_signature_type           hmac;
    simlock_rsa_signature_type            rsa;
  } data;
}simlock_rsu_config_auth_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      SIMLOCK_RSU_CONFIG_DATA_TYPE

   DESCRIPTION:
     This structure contains the configuration data

     category_header  : header data
     code_data        : code data
-------------------------------------------------------------------------------*/
typedef struct
{
  simlock_rsu_config_header_data_type    header_data;
  simlock_rsu_config_code_data_type      code_data;
  simlock_imei_type                      imei;
  simlock_rsu_config_auth_data_type      auth_data;
} simlock_rsu_config_data_type;

#ifdef FEATURE_SIMLOCK_RSU
#define SIMLOCK_RSU_CONFIG_RFU_SIZE                         2
#define SIMLOCK_RSU_CONFIG_IMEI_LEN                        16
#define SIMLOCK_RSU_CONFIG_MAJOR_VERSION_ONE                1
#define SIMLOCK_RSU_CONFIG_MINOR_VERSION_MIN                5

/* ----------------------------------------------------------------------------
   STRUCTURE:      SIMLOCK_RSU_CONFIG_MCC_MNC_LIST_TYPE

   DESCRIPTION:
     This structure contains the list of MCC/MNCs

     num_mcc_mnc_codes   : Number of MCC/MNCs in the list
     mcc_mnc_code_ptr    : Pointer to MCC/MNC data
-------------------------------------------------------------------------------*/
typedef struct
{
  uint16                              num_mcc_mnc_codes;
  simlock_rsu_config_mcc_mnc_type   * mcc_mnc_code_ptr;
}simlock_rsu_config_mcc_mnc_list_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      SIMLOCK_RSU_CONFIG_GID_LIST_TYPE

   DESCRIPTION:
     This structure contains the list of GIDs

     num_gid_codes       : Number of GIDs in the list
     gid_code_ptr        : Pointer to GID data
-------------------------------------------------------------------------------*/
typedef struct
{
  uint16                              num_gid_codes;
  simlock_rsu_config_gid_type       * gid_code_ptr;
}simlock_rsu_config_gid_list_type;

static const simlock_rsu_config_mcc_mnc_type simlock_rsu_always_present_codes[] = {0x31016000, 0x31020000, 0x31021000, 0x31022000,
                                                                                   0x31023000, 0x31024000, 0x31025000, 0x31026000,
                                                                                   0x31027000, 0x31030000, 0x31031000, 0x31049000,
                                                                                   0x31053000, 0x31058000, 0x31059000, 0x31064000,
                                                                                   0x31066000, 0x31080000, 0x00101000};

static const simlock_mcc_type simlock_rsu_test_mcc = {'0', '0', '1'};

#elif defined(FEATURE_SIMLOCK_QC_REMOTE_LOCK)

#define SIMLOCK_REMOTE_LOCK_CONFIG_MAJOR_VERSION            1
#define SIMLOCK_REMOTE_LOCK_CONFIG_MINOR_VERSION            0

#define SIMLOCK_REMOTE_LOCK_MIN_CODE_DATA_LEN               3
#define SIMLOCK_REMOTE_LOCK_NUM_CODES_LENGTH                1

#define SIMLOCK_REMOTE_LOCK_NW_CODE_DATA_SIZE               6
#define SIMLOCK_REMOTE_LOCK_NS_CODE_DATA_SIZE               8
#define SIMLOCK_REMOTE_LOCK_SP_CODE_DATA_SIZE               7

/* ----------------------------------------------------------------------------
   STRUCTURE:      SIMLOCK_REMOTE_LOCK_TIMESTAMP_TYPE

   DESCRIPTION:
     This structure contains the time data related to the last processed
     request. Time in milliseconds.

     version             : File version
     timestamp           : Time stamp when blob is created
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST {
  uint8     version;
  uint64    timestamp;
} simlock_remote_lock_timestamp_type;
#endif /* FEATURE_SIMLOCK_RSU || FEATURE_SIMLOCK_QC_REMOTE_LOCK */

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================
FUNCTION SIMLOCK_RSU_CONFIG_FREE_CODE_DATA

DESCRIPTION
  This function frees the code data in the config data

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
static void simlock_rsu_config_free_code_data
(
  simlock_rsu_config_data_type    * config_data_ptr
)
{
  if(config_data_ptr == NULL)
  {
    return;
  }

  if(config_data_ptr->code_data.lock_code_data.nw_lock.nw_code_ptr != NULL)
  {
    SIMLOCK_MEM_FREE(config_data_ptr->code_data.lock_code_data.nw_lock.nw_code_ptr);
  }
  if(config_data_ptr->code_data.lock_code_data.ns_lock.ns_code_ptr != NULL)
  {
    SIMLOCK_MEM_FREE(config_data_ptr->code_data.lock_code_data.ns_lock.ns_code_ptr);
  }
  if(config_data_ptr->code_data.lock_code_data.sp_lock.sp_code_ptr != NULL)
  {
    SIMLOCK_MEM_FREE(config_data_ptr->code_data.lock_code_data.sp_lock.sp_code_ptr);
  }
  if(config_data_ptr->auth_data.msg_data_ptr != NULL)
  {
    SIMLOCK_MEM_FREE(config_data_ptr->auth_data.msg_data_ptr);
  }

  SIMLOCK_MEM_FREE(config_data_ptr);
} /* simlock_rsu_config_free_code_data */


/*===========================================================================
FUNCTION SIMLOCK_RSU_CONFIG_PARSE_PROTECTION_TYPE

DESCRIPTION
  This function converts the protection type value in configuration data to
  the corresponding enum type

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_rsu_config_parse_protection_type
(
  uint8                                        parsed_value,
  simlock_rsu_config_protection_enum_type   *  protection_type_ptr
)
{
  if(protection_type_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

#ifdef FEATURE_SIMLOCK_RSU
  switch(parsed_value)
  {
    case 0:
      *protection_type_ptr = SIMLOCK_RSU_CONFIG_PROTECTION_NONE;
      break;

    case 1:
      *protection_type_ptr = SIMLOCK_RSU_CONFIG_PROTECTION_SHA256;
      break;

    case 2:
      *protection_type_ptr = SIMLOCK_RSU_CONFIG_PROTECTION_RSA;
      break;

    default:
      return SIMLOCK_GENERIC_ERROR;
  }
#elif defined(FEATURE_SIMLOCK_QC_REMOTE_LOCK)
  switch(parsed_value)
  {
    case 0:
      *protection_type_ptr = SIMLOCK_RSU_CONFIG_PROTECTION_NONE;
      break;

    case 1:
      *protection_type_ptr = SIMLOCK_RSU_CONFIG_PROTECTION_RSA;
       break;

    default:
      return SIMLOCK_GENERIC_ERROR;
  }
#endif /* FEATURE_SIMLOCK_RSU || FEATURE_SIMLOCK_QC_REMOTE_LOCK */

  return SIMLOCK_SUCCESS;
}/* simlock_rsu_config_parse_protection_type */


/*===========================================================================
FUNCTION SIMLOCK_RSU_CONFIG_PARSE_LOCK_STATE

DESCRIPTION
  This function converts the lock state value in configuration data to
  the corresponding enum type

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_rsu_config_parse_lock_state
(
  uint8                                        parsed_value,
  simlock_rsu_config_lock_state_enum_type   *  lock_state_ptr
)
{
  if(lock_state_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

#ifdef FEATURE_SIMLOCK_RSU
  switch(parsed_value)
  {
    case 0:
      *lock_state_ptr = SIMLOCK_RSU_CONFIG_LOCK_STATE_PERMANENT_LOCK;
      break;

    case 1:
      *lock_state_ptr = SIMLOCK_RSU_CONFIG_LOCK_STATE_TEMPORARY_UNLOCK;
      break;

    case 2:
      *lock_state_ptr = SIMLOCK_RSU_CONFIG_LOCK_STATE_PERMANENT_UNLOCK;
      break;

    case 3:
      *lock_state_ptr = SIMLOCK_RSU_CONFIG_LOCK_STATE_PARTIAL_UNLOCK;
      break;

    default:
      return SIMLOCK_GENERIC_ERROR;
  }
#elif defined(FEATURE_SIMLOCK_QC_REMOTE_LOCK)
  switch(parsed_value)
  {
    case 0:
      *lock_state_ptr = SIMLOCK_RSU_CONFIG_LOCK_STATE_PERMANENT_LOCK;
      break;

    case 1:
      *lock_state_ptr = SIMLOCK_RSU_CONFIG_LOCK_STATE_PERMANENT_UNLOCK;
      break;

    default:
      return SIMLOCK_GENERIC_ERROR;
  }
#endif /* FEATURE_SIMLOCK_RSU || FEATURE_SIMLOCK_QC_REMOTE_LOCK */

  return SIMLOCK_SUCCESS;
}/* simlock_rsu_config_parse_lock_state */


/*===========================================================================
FUNCTION SIMLOCK_RSU_CONFIG_GET_LOCK_STATUS

DESCRIPTION
  This function gets the status of the locks (enabled or disabled)

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_rsu_config_get_lock_status
(
  simlock_message_response_data_type  * lock_status_resp_ptr
)
{
  simlock_result_enum_type              simlock_status       = SIMLOCK_SUCCESS;
  simlock_message_request_data_type   * lock_status_req_ptr  = NULL;

  if(lock_status_resp_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  SIMLOCK_CHECK_AND_MEM_MALLOC(lock_status_req_ptr,
                               sizeof(simlock_message_request_data_type));
  if (lock_status_req_ptr == NULL)
  {
    return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  lock_status_req_ptr->msg_type  = SIMLOCK_GET_STATUS_MSG;

  /* call the engine with get status request */
  simlock_status = simlock_process_message(lock_status_req_ptr, lock_status_resp_ptr);

  SIMLOCK_MEM_FREE(lock_status_req_ptr);

  return simlock_status;
} /* simlock_rsu_config_get_lock_status */


/*===========================================================================
FUNCTION SIMLOCK_RSU_CONFIG_SEND_UNLOCK_SECURED

DESCRIPTION
  This function sends the unlock request to the simlock engine. This is
  a secured unlock request and so there is no CK

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_rsu_config_send_unlock_secured
(
  simlock_slot_enum_type        slot,
  simlock_category_enum_type    category
)
{
  simlock_result_enum_type              simlock_status  = SIMLOCK_SUCCESS;
  simlock_message_request_data_type   * unlock_req_ptr  = NULL;
  simlock_message_response_data_type  * unlock_resp_ptr = NULL;

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

  unlock_req_ptr->msg_type                                = SIMLOCK_UNLOCK_DEVICE_MSG;
  unlock_req_ptr->message.unlock_msg.slot                 = slot;
  unlock_req_ptr->message.unlock_msg.category             = category;
  unlock_req_ptr->message.unlock_msg.ck.simlock_ck_length = 0;

  memset(unlock_req_ptr->message.unlock_msg.ck.simlock_ck_data, 0x00,
         sizeof(unlock_req_ptr->message.unlock_msg.ck.simlock_ck_data));

  /* call the engine with unlock request */
  simlock_status = simlock_process_message(unlock_req_ptr, unlock_resp_ptr);

  SIMLOCK_MEM_FREE(unlock_req_ptr);
  SIMLOCK_MEM_FREE(unlock_resp_ptr);

  return simlock_status;
} /* simlock_rsu_config_send_unlock_secured */


/*===========================================================================
FUNCTION SIMLOCK_RSU_CONFIG_PROCESS_PERMANENT_UNLOCK

DESCRIPTION
  This function processes the permanent unlock of categories locked

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_rsu_config_process_permanent_unlock
(
  void
)
{
  simlock_result_enum_type              simlock_status      = SIMLOCK_SUCCESS;
  simlock_category_enum_type            category            = SIMLOCK_CATEGORY_3GPP_NW;
  uint8                                 category_index      = 0;
  simlock_message_response_data_type  * get_status_resp_ptr = NULL;

  SIMLOCK_CHECK_AND_MEM_MALLOC(get_status_resp_ptr,
                               sizeof(simlock_message_response_data_type));
  if (get_status_resp_ptr == NULL)
  {
    return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* get the lock status */
  simlock_status = simlock_rsu_config_get_lock_status(get_status_resp_ptr);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    SIMLOCK_MEM_FREE(get_status_resp_ptr);
    return simlock_status;
  }

  /* send the unlock request.
     3GPP_SIM category is normally controlled by SafeSwitch module, while all
     other categories are under the control of operator. We don't want RSU to
     modify 3GPP_SIM. */
  for(category = SIMLOCK_CATEGORY_3GPP_NW;
      category < SIMLOCK_CATEGORY_3GPP_SIM;
      category++)
  {
    simlock_status = simlock_util_get_category_index(category, &category_index);
    if((simlock_status == SIMLOCK_SUCCESS) &&
       (category_index < SIMLOCK_CATEGORY_COUNT_MAX))
    {
      /* if the category is not ebabled, then no need to unlock.
         Check the status corresponding to slot-1 (zero index) only as
         we lock categories on slot-1 */
      if(!get_status_resp_ptr->message.get_status_resp.lock_info[0].category_info[category_index].category_status)
      {
        continue;
      }

      simlock_status = simlock_rsu_config_send_unlock_secured(SIMLOCK_SLOT_1, category);
      if (simlock_status != SIMLOCK_SUCCESS)
      {
        break;
      }

      /* Unlock is successful for this category.
         Check if this is temporarily unlocked previously and now successfully unlocked.
         If yes, send temporary event so that this would notify the end indication to clients */
      if(get_status_resp_ptr->message.get_status_resp.remaining_time.duration[0][category_index] > 0)
      {
        simlock_send_temporary_unlock_event(SIMLOCK_SLOT_1, category);
      }
    }
  } /* end loop categories */

  if(simlock_status == SIMLOCK_SUCCESS)
  {
    /* If permanent unlock is success, then all the categories are unlocked.
       Hence, clear the contents of temp unlock request from the sfs */
    simlock_temp_unlock_reset_config(SIMLOCK_TEMP_UNLOCK_RESET_CONFIG_IF_UNLOCKED);
  }

  SIMLOCK_MEM_FREE(get_status_resp_ptr);
  return simlock_status;
} /* simlock_rsu_config_process_permanent_unlock */


/*===========================================================================
FUNCTION SIMLOCK_RSU_CONFIG_SEND_LOCK

DESCRIPTION
  This function sends the lock request to the engine

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_rsu_config_send_lock
(
  simlock_slot_enum_type           slot,
  simlock_category_data_type   *   category_data_ptr
)
{
  simlock_result_enum_type              simlock_status  = SIMLOCK_SUCCESS;
  simlock_message_request_data_type   * lock_req_ptr    = NULL;

  if (category_data_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  SIMLOCK_CHECK_AND_MEM_MALLOC(lock_req_ptr,
                               sizeof(simlock_message_request_data_type));
  if (lock_req_ptr == NULL)
  {
    return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  lock_req_ptr->msg_type                                 = SIMLOCK_SET_LOCK_CK_MSG;
  lock_req_ptr->message.lock_ck_msg.slot                 = slot;
  lock_req_ptr->message.lock_ck_msg.category             = *category_data_ptr;
  lock_req_ptr->message.lock_ck_msg.auto_lock            = FALSE;
  lock_req_ptr->message.lock_ck_msg.blacklist            = FALSE;
  lock_req_ptr->message.lock_ck_msg.num_retries_max      = 0;
  lock_req_ptr->message.lock_ck_msg.iteration_cnt        = 0;
  lock_req_ptr->message.lock_ck_msg.ck.simlock_ck_length = 0;

  memset(lock_req_ptr->message.lock_ck_msg.ck.simlock_ck_data, 0x00,
         sizeof(lock_req_ptr->message.lock_ck_msg.ck.simlock_ck_data));

  /* call the engine with lock request */
  simlock_status = simlock_process_message(lock_req_ptr, NULL);

  SIMLOCK_MEM_FREE(lock_req_ptr);

  return simlock_status;
} /* simlock_rsu_config_send_lock */


#ifdef FEATURE_SIMLOCK_RSU
/*===========================================================================
FUNCTION SIMLOCK_RSU_CONFIG_IS_MCC_MNC_IN_ALWAYS_PRESENT_LIST

DESCRIPTION
  This function checks if the mcc-mnc is present in the list of mcc-mnc
  that are always added in the code data

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static boolean simlock_rsu_config_is_mcc_mnc_in_always_present_list
(
  simlock_rsu_config_mcc_mnc_type      mcc_mnc
)
{
  uint8       list_cnt = 0;

  for (list_cnt = 0; list_cnt < (sizeof(simlock_rsu_always_present_codes)/sizeof(simlock_rsu_config_mcc_mnc_type));
       list_cnt++)
  {
    if(simlock_rsu_always_present_codes[list_cnt] == mcc_mnc)
    {
      return TRUE;
    }
  }

  return FALSE;
} /* simlock_rsu_config_is_mcc_mnc_in_always_present_list */


/*===========================================================================
FUNCTION SIMLOCK_RSU_CONFIG_CONVERT_GID

DESCRIPTION
  This function converts the gid data in the configuration data to the
  simlock gid type

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_rsu_config_convert_gid
(
  simlock_rsu_config_gid_type          gid,
  simlock_gid_type                  *  gid_data_ptr
)
{
  if(gid_data_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  gid_data_ptr->gid_len = SIMLOCK_RSU_CONFIG_GID_SIZE;

  gid_data_ptr->gid_data[0] = (uint8)((gid & SIMLOCK_RSU_CONFIG_HIGH_BYTE) >> 8);
  gid_data_ptr->gid_data[1] = (uint8)(gid & SIMLOCK_RSU_CONFIG_LOW_BYTE);

  return SIMLOCK_SUCCESS;
} /* simlock_rsu_config_convert_gid */


/*===========================================================================
FUNCTION SIMLOCK_RSU_CONFIG_CONVERT_MCC_MNC

DESCRIPTION
  This function converts the MCC/MNC data in the configuration data to the
  simlock NW type

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_rsu_config_convert_mcc_mnc
(
  simlock_rsu_config_mcc_mnc_type      mcc_mnc,
  simlock_nw_code_data_type         *  mcc_mnc_data_ptr
)
{
  uint8      temp_data = 0;

  if(mcc_mnc_data_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  temp_data = (uint8)((mcc_mnc & 0xFF000000) >> 24);

  mcc_mnc_data_ptr->mcc[0] = (((temp_data & SIMLOCK_RSU_CONFIG_HIGH_NIBBLE) >> 4) + '0');
  mcc_mnc_data_ptr->mcc[1] = ((temp_data & SIMLOCK_RSU_CONFIG_LOW_NIBBLE) + '0');

  temp_data = (uint8)((mcc_mnc & 0x00FF0000) >> 16);

  mcc_mnc_data_ptr->mcc[2] = (((temp_data & SIMLOCK_RSU_CONFIG_HIGH_NIBBLE) >> 4) + '0');

  if((temp_data & SIMLOCK_RSU_CONFIG_LOW_NIBBLE) == 0x0F)
  {
    mcc_mnc_data_ptr->mnc.mnc_len = 2;

    temp_data = (uint8)((mcc_mnc & 0x0000FF00) >> 8);
    mcc_mnc_data_ptr->mnc.mnc_data[0] = (((temp_data & SIMLOCK_RSU_CONFIG_HIGH_NIBBLE) >> 4) + '0');
    mcc_mnc_data_ptr->mnc.mnc_data[1] = ((temp_data & SIMLOCK_RSU_CONFIG_LOW_NIBBLE) + '0');
  }
  else
  {
    /* The correct coding for 2-digit MNC is 0x001F0100.
       But the test mcc-mnc is coded in blob as 0x00101000. */
    if((memcmp(simlock_rsu_test_mcc, mcc_mnc_data_ptr->mcc,
               SIMLOCK_MCC_LEN) == 0))
    {
      mcc_mnc_data_ptr->mnc.mnc_len = 2;

      mcc_mnc_data_ptr->mnc.mnc_data[0] = ((temp_data & SIMLOCK_RSU_CONFIG_LOW_NIBBLE) + '0');

      temp_data = (uint8)((mcc_mnc & 0x0000FF00) >> 8);
      mcc_mnc_data_ptr->mnc.mnc_data[1] = (((temp_data & SIMLOCK_RSU_CONFIG_HIGH_NIBBLE) >> 4) + '0');
    }
    else
    {
      mcc_mnc_data_ptr->mnc.mnc_len = 3;
      mcc_mnc_data_ptr->mnc.mnc_data[0] = ((temp_data & SIMLOCK_RSU_CONFIG_LOW_NIBBLE) + '0');

      temp_data = (uint8)((mcc_mnc & 0x0000FF00) >> 8);
      mcc_mnc_data_ptr->mnc.mnc_data[1] = (((temp_data & SIMLOCK_RSU_CONFIG_HIGH_NIBBLE) >> 4) + '0');
      mcc_mnc_data_ptr->mnc.mnc_data[2] = ((temp_data & SIMLOCK_RSU_CONFIG_LOW_NIBBLE) + '0');
    }
  }

  return SIMLOCK_SUCCESS;
} /* simlock_rsu_config_convert_mcc_mnc */
#endif /* FEATURE_SIMLOCK_RSU */

/*===========================================================================
FUNCTION SIMLOCK_RSU_CONFIG_BUILD_CATEGORY_DATA

DESCRIPTION
  This function builds the code data for a category

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_rsu_config_build_category_data
(
  const simlock_rsu_config_data_type *  config_data_ptr,
  simlock_category_enum_type            category,
  simlock_category_data_type         *  category_data_ptr
)
{
  uint16  code_cnt = 0;

  if((config_data_ptr == NULL) ||
     (category_data_ptr == NULL))
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  category_data_ptr->category_type = category;

  switch(category)
  {
    case SIMLOCK_CATEGORY_3GPP_NW:
      category_data_ptr->code_data.nw_3gpp_code_list.num_nw_codes = 0;
      if(config_data_ptr->code_data.lock_code_data.nw_lock.nw_code_ptr != NULL)
      {
        for(code_cnt = 0; code_cnt < config_data_ptr->code_data.lock_code_data.nw_lock.nw_codes &&
                          code_cnt < SIMLOCK_NUM_CODES_MAX; code_cnt++)
        {
          category_data_ptr->code_data.nw_3gpp_code_list.num_nw_codes++;
          category_data_ptr->code_data.nw_3gpp_code_list.nw_code_data[code_cnt] = 
            config_data_ptr->code_data.lock_code_data.nw_lock.nw_code_ptr[code_cnt];
        }
      }
      break;

    case SIMLOCK_CATEGORY_3GPP_NS:
      category_data_ptr->code_data.ns_3gpp_code_list.num_ns_codes = 0;
      if(config_data_ptr->code_data.lock_code_data.ns_lock.ns_code_ptr != NULL)
      {
        for(code_cnt = 0; code_cnt < config_data_ptr->code_data.lock_code_data.ns_lock.ns_codes &&
                          code_cnt < SIMLOCK_NUM_CODES_MAX; code_cnt++)
        {
          category_data_ptr->code_data.ns_3gpp_code_list.num_ns_codes++;
          category_data_ptr->code_data.ns_3gpp_code_list.ns_code_data[code_cnt] = 
            config_data_ptr->code_data.lock_code_data.ns_lock.ns_code_ptr[code_cnt];
        }
      }
      break;

    case SIMLOCK_CATEGORY_3GPP_SP:
      category_data_ptr->code_data.sp_3gpp_code_list.num_sp_codes = 0;
      if(config_data_ptr->code_data.lock_code_data.sp_lock.sp_code_ptr != NULL)
      {
        for(code_cnt = 0; code_cnt < config_data_ptr->code_data.lock_code_data.sp_lock.sp_codes &&
                          code_cnt < SIMLOCK_NUM_CODES_MAX; code_cnt++)
        {
          category_data_ptr->code_data.sp_3gpp_code_list.num_sp_codes++;
          category_data_ptr->code_data.sp_3gpp_code_list.sp_code_data[code_cnt] = 
            config_data_ptr->code_data.lock_code_data.sp_lock.sp_code_ptr[code_cnt];
        }
      }
      break;

    default:
      return SIMLOCK_GENERIC_ERROR;
  }

  return SIMLOCK_SUCCESS;
} /* simlock_rsu_config_build_category_data */


/*===========================================================================
FUNCTION SIMLOCK_RSU_CONFIG_PROCESS_PARTIAL_LOCK

DESCRIPTION
  This function processes the permanent lock of categories. In this case
  only NW lock is set

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_rsu_config_process_partial_lock
(
  const simlock_rsu_config_data_type    * config_data_ptr
)
{
  simlock_result_enum_type       simlock_status    = SIMLOCK_SUCCESS;
  simlock_category_data_type    *category_data_ptr = NULL;

  if (config_data_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  SIMLOCK_CHECK_AND_MEM_MALLOC(category_data_ptr, sizeof(simlock_category_data_type));
  if(category_data_ptr == NULL)
  {
    return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* send unlock, in case there are any locks sets */
  simlock_status = simlock_rsu_config_process_permanent_unlock();

  /* apply the NW lock based on the PLMNs received in the
     configuration data */
  if(config_data_ptr->code_data.lock_code_data.nw_lock.nw_codes > 0)
  {
    simlock_status = simlock_rsu_config_build_category_data(config_data_ptr,
                                                            SIMLOCK_CATEGORY_3GPP_NW,
                                                            category_data_ptr);
    if(simlock_status == SIMLOCK_SUCCESS)
    {
      simlock_status = simlock_rsu_config_send_lock(SIMLOCK_SLOT_1,
                                                    category_data_ptr);
    }
  }

  SIMLOCK_MEM_FREE(category_data_ptr);
  return simlock_status;
} /* simlock_rsu_config_process_partial_lock */


/*===========================================================================
FUNCTION SIMLOCK_RSU_CONFIG_PROCESS_PERMANENT_LOCK

DESCRIPTION
  This function processes the permanent lock of categories. In this case
  both NW and SP locks are set

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_rsu_config_process_permanent_lock
(
  const simlock_rsu_config_data_type    * config_data_ptr
)
{
  simlock_result_enum_type       simlock_status    = SIMLOCK_SUCCESS;
  simlock_category_data_type    *category_data_ptr = NULL;

  if (config_data_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  SIMLOCK_CHECK_AND_MEM_MALLOC(category_data_ptr, sizeof(simlock_category_data_type));
  if(category_data_ptr == NULL)
  {
    return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* send unlock, in case there are any locks sets */
  simlock_status = simlock_rsu_config_process_permanent_unlock();

  /* Apply the NW lock */
  if(config_data_ptr->code_data.lock_code_data.nw_lock.nw_codes > 0)
  {
    simlock_status = simlock_rsu_config_build_category_data(config_data_ptr,
                                                            SIMLOCK_CATEGORY_3GPP_NW,
                                                            category_data_ptr);
    if(simlock_status == SIMLOCK_SUCCESS)
    {
      simlock_status = simlock_rsu_config_send_lock(SIMLOCK_SLOT_1,
                                                    category_data_ptr);
    }
  }

  /* Apply the NS lock */
  if(config_data_ptr->code_data.lock_code_data.ns_lock.ns_codes > 0)
  {
    memset(category_data_ptr, 0x00, sizeof(simlock_category_data_type));

    simlock_status = simlock_rsu_config_build_category_data(config_data_ptr,
                                                            SIMLOCK_CATEGORY_3GPP_NS,
                                                            category_data_ptr);
    if(simlock_status == SIMLOCK_SUCCESS)
    {
      simlock_status = simlock_rsu_config_send_lock(SIMLOCK_SLOT_1,
                                                    category_data_ptr);
    }
  }

  /* Apply the SP lock */
  if(config_data_ptr->code_data.lock_code_data.sp_lock.sp_codes > 0)
  {
    memset(category_data_ptr, 0x00, sizeof(simlock_category_data_type));

    simlock_status = simlock_rsu_config_build_category_data(config_data_ptr,
                                                            SIMLOCK_CATEGORY_3GPP_SP,
                                                            category_data_ptr);

    if(simlock_status == SIMLOCK_SUCCESS)
    {
      simlock_status = simlock_rsu_config_send_lock(SIMLOCK_SLOT_1,
                                                    category_data_ptr);
    }
  }

  SIMLOCK_MEM_FREE(category_data_ptr);
  return simlock_status;
} /* simlock_rsu_config_process_permanent_lock */


/*===========================================================================
FUNCTION SIMLOCK_RSU_CONFIG_PROCESS_TEMPORARY_UNLOCK

DESCRIPTION
  This function processes the temporary unlock of categories locked

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_rsu_config_process_temporary_unlock
(
  const simlock_rsu_config_data_type    * config_data_ptr
)
{
  simlock_result_enum_type              simlock_status      = SIMLOCK_SUCCESS;
  simlock_category_enum_type            category            = SIMLOCK_CATEGORY_3GPP_NW;
  uint8                                 category_index      = 0;
  simlock_message_response_data_type  * get_status_resp_ptr = NULL;

  if (config_data_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  /* apply the configuration present in the temporary unlock 
     request before starting the temporary unlock.
     This configuration will be active after the temporary
     unlock has expired */
  simlock_status = simlock_rsu_config_process_permanent_lock(config_data_ptr);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    return simlock_status;
  }

  SIMLOCK_CHECK_AND_MEM_MALLOC(get_status_resp_ptr,
                               sizeof(simlock_message_response_data_type));
  if (get_status_resp_ptr == NULL)
  {
    return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* get the lock status */
  simlock_status = simlock_rsu_config_get_lock_status(get_status_resp_ptr);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    SIMLOCK_MEM_FREE(get_status_resp_ptr);
    return simlock_status;
  }

  /* send the temporary unlock request.
     3GPP_SIM category is normally controlled by SafeSwitch module, while all
     other categories are under the control of operator. We don't want RSU to
     modify 3GPP_SIM. */
  for(category = SIMLOCK_CATEGORY_3GPP_NW;
      category < SIMLOCK_CATEGORY_3GPP_SIM;
      category++)
  {
    simlock_status = simlock_util_get_category_index(category, &category_index);
    if((simlock_status == SIMLOCK_SUCCESS) &&
       (category_index < SIMLOCK_CATEGORY_COUNT_MAX))
    {
      /* if the category is not enabled, then no need to temporary unlock.
         Check the status corresponding to slot-1 (zero index) only as
         we lock categories on slot-1 */
      if(!get_status_resp_ptr->message.get_status_resp.lock_info[0].category_info[category_index].category_status)
      {
        continue;
      }

      simlock_status = simlock_temp_unlock_process_request(SIMLOCK_SLOT_1,
                                                           category,
                                                           config_data_ptr->code_data.start_time,
                                                           config_data_ptr->code_data.end_time);
      if (simlock_status != SIMLOCK_SUCCESS)
      {
        break;
      }
    }
  } /* end loop categories */

  SIMLOCK_MEM_FREE(get_status_resp_ptr);
  return simlock_status;
} /* simlock_rsu_config_process_temporary_unlock */


/*===========================================================================
FUNCTION SIMLOCK_RSU_CONFIG_APPLY_CONFIGURATION

DESCRIPTION
  This function applies configuration requested in the RSU data

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_rsu_config_apply_configuration
(
  const simlock_rsu_config_data_type    * config_data_ptr
)
{
  simlock_result_enum_type    simlock_status  = SIMLOCK_SUCCESS;

  if (config_data_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  switch(config_data_ptr->header_data.lock_state)
  {
    case SIMLOCK_RSU_CONFIG_LOCK_STATE_PERMANENT_LOCK:
      simlock_status = simlock_rsu_config_process_permanent_lock(config_data_ptr);
      break;

    case SIMLOCK_RSU_CONFIG_LOCK_STATE_TEMPORARY_UNLOCK:
      simlock_status = simlock_rsu_config_process_temporary_unlock(config_data_ptr);
      break;

    case SIMLOCK_RSU_CONFIG_LOCK_STATE_PERMANENT_UNLOCK:
      simlock_status = simlock_rsu_config_process_permanent_unlock();
      break;

    case SIMLOCK_RSU_CONFIG_LOCK_STATE_PARTIAL_UNLOCK:
      simlock_status = simlock_rsu_config_process_partial_lock(config_data_ptr);
      break;

    default:
      simlock_status = SIMLOCK_GENERIC_ERROR;
      break;
  }

  return simlock_status;
} /* simlock_rsu_config_apply_configuration */


/*===========================================================================
FUNCTION SIMLOCK_RSU_CONFIG_CHECK_IMEI

DESCRIPTION
  This function checks if the IMEI received in the simlock configuration data
  matches the device IMEI

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_rsu_config_check_imei
(
  simlock_imei_type    imei_data
)
{
  simlock_slot_enum_type    slot            = SIMLOCK_SLOT_1;
  simlock_imei_type         imei;

  memset(&imei, 0x00, sizeof(simlock_imei_type));

  /* Get the IMEI for each slot/context */
  for(slot = SIMLOCK_SLOT_1; slot < SIMLOCK_SLOT_NUM; slot++)
  {
    if(simlock_util_check_slot(slot)      != SIMLOCK_SUCCESS ||
       simlock_util_get_imei(slot, &imei) != SIMLOCK_SUCCESS ||
       imei.imei_len == 0)
    {
      continue;
    }
  
    /* check if the imei matches */
    if(simlock_util_check_imei_matches(imei, imei_data))
    {
      return SIMLOCK_SUCCESS;
    }
  }
  return SIMLOCK_GENERIC_ERROR;
} /* simlock_rsu_config_check_imei */


/*===========================================================================
FUNCTION SIMLOCK_RSU_CONFIG_VERIFY_DATA_INTEGRITY

DESCRIPTION
  This function verifies the integrity of simlock configuration data by
  comparing the macs

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_rsu_config_verify_data_integrity
(
  const simlock_rsu_config_data_type    * config_data_ptr
)
{
  simlock_result_enum_type        simlock_status   = SIMLOCK_SUCCESS;
#ifdef FEATURE_SIMLOCK_RSU
  simlock_rsu_keymod_type         key_mod_data_buf;
  simlock_hmac_signature_type     hmac_data_buf;
#elif defined(FEATURE_SIMLOCK_QC_REMOTE_LOCK)
  simlock_data_type               msg_digest_info  = {0, NULL};
  simlock_rsu_pubkey_data_type   *pubkey_ptr       = NULL;
#endif /* FEATURE_SIMLOCK_RSU || FEATURE_SIMLOCK_QC_REMOTE_LOCK */

  if (config_data_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

#ifdef FEATURE_QMI_UIM_SECURE_CHANNEL_TEST
  #error code not present
#endif /* FEATURE_QMI_UIM_SECURE_CHANNEL_TEST */

  /* If there is a protection to the blob, msg_len should not be zero
     And in production environment, no protection is not allowed */
  if((config_data_ptr->auth_data.msg_len == 0) ||
     (config_data_ptr->auth_data.msg_data_ptr == NULL))
  {
    return SIMLOCK_GENERIC_ERROR;
  }

#ifdef FEATURE_SIMLOCK_RSU
  if(config_data_ptr->header_data.protection_type != SIMLOCK_RSU_CONFIG_PROTECTION_SHA256)
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  memset(key_mod_data_buf, 0x00, SIMLOCK_RSU_KEY_MOD_LEN);
  memset(hmac_data_buf, 0x00, SIMLOCK_HMAC_SHA256_LEN);

  /* read the key-mod from file */
  simlock_status = simlock_rsu_key_read_keymod_file(key_mod_data_buf);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    return simlock_status;
  }

  /* generate the hmac */
  simlock_status = simlock_crypto_generate_hmac_sha256(config_data_ptr->auth_data.msg_data_ptr,
                                                       config_data_ptr->auth_data.msg_len,
                                                       key_mod_data_buf,
                                                       SIMLOCK_RSU_KEY_MOD_LEN,
                                                       hmac_data_buf);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    return simlock_status;
  }

  if(memcmp(hmac_data_buf, config_data_ptr->auth_data.data.hmac,
            SIMLOCK_HMAC_SHA256_LEN) != 0)
  {
    simlock_status = SIMLOCK_GENERIC_ERROR;
  }
#elif defined(FEATURE_SIMLOCK_QC_REMOTE_LOCK)
  if(config_data_ptr->header_data.protection_type != SIMLOCK_RSU_CONFIG_PROTECTION_RSA)
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  SIMLOCK_CHECK_AND_MEM_MALLOC(pubkey_ptr, sizeof(simlock_rsu_pubkey_data_type));
  if(pubkey_ptr == NULL)
  {
    return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  simlock_status = simlock_rsu_key_read_pubkey_data(pubkey_ptr);

  if(simlock_status != SIMLOCK_SUCCESS)
  {
    SIMLOCK_FREE(pubkey_ptr);
    return simlock_status;
  }

  simlock_status = simlock_crypto_generate_digest_sha256(config_data_ptr->auth_data.msg_data_ptr,
                                                         config_data_ptr->auth_data.msg_len,
                                                         &msg_digest_info);

  if(simlock_status == SIMLOCK_SUCCESS)
  {
    uint8 *rsa_ptr = NULL;

    SIMLOCK_CHECK_AND_MEM_MALLOC(rsa_ptr, sizeof(simlock_rsa_signature_type));
    if(rsa_ptr == NULL)
    {
      SIMLOCK_FREE(pubkey_ptr);
      return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
    }

    simlock_memscpy(rsa_ptr,
                    sizeof(simlock_rsa_signature_type),
                    config_data_ptr->auth_data.data.rsa,
                    sizeof(simlock_rsa_signature_type));

    /* Validating the signature is as follows 
       Blob request would have a message(M) appended with signature(S). 
       a. Generate the message hash(H) from the message(M) in blob request using SHA256
       b. Give H generated above and Signature (S) along with Public Key to secapi_pkx_authenticate()
       c. Success response for above API means signature (S) is generated for the message (M)
          and the data integrity of the blob is verified */
    simlock_status = simlock_crypto_rsa(SIMLOCK_CRYPTO_RSA_SIGNATURE_VALIDATE,
                                        rsa_ptr,
                                        SIMLOCK_RSA_LEN,
                                        pubkey_ptr->modulus,
                                        SIMLOCK_RSU_PUBKEY_MODULUS_LEN,
                                        pubkey_ptr->exponent,
                                        SIMLOCK_RSU_PUBKEY_EXPONENT_LEN,
                                        &msg_digest_info);

    SIMLOCK_FREE(rsa_ptr);
  }

  SIMLOCK_FREE(pubkey_ptr);
  SIMLOCK_MEM_FREE(msg_digest_info.data_ptr);
#endif /* FEATURE_SIMLOCK_RSU || FEATURE_SIMLOCK_QC_REMOTE_LOCK */

  return simlock_status;
} /* simlock_rsu_config_verify_data_integrity */


#ifdef FEATURE_SIMLOCK_QC_REMOTE_LOCK
/*===========================================================================
FUNCTION SIMLOCK_REMOTE_LOCK_CONVERT_MCC_MNC

DESCRIPTION
  This function converts the mcc mnc to the required format.
  data_ptr should be a buffer of minimum 6 bytes and this is to be ensured by caller.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type  simlock_remote_lock_convert_mcc_mnc(
  const uint8                 *data_ptr,
  uint8                        data_len,
  simlock_nw_code_data_type   *mcc_mnc_ptr
)
{
  uint8 offset = 0;
  uint8 i      = 0;

  if(data_ptr == NULL ||
     data_len < SIMLOCK_REMOTE_LOCK_NW_CODE_DATA_SIZE ||
     mcc_mnc_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  for(i = 0; (i < SIMLOCK_MCC_LEN && offset < data_len); i++)
  {
    mcc_mnc_ptr->mcc[i] = data_ptr[offset++];
  }

  mcc_mnc_ptr->mnc.mnc_len = 3;

  for(i = 0; (i < SIMLOCK_MNC_MAX && offset < data_len); i++)
  {
    mcc_mnc_ptr->mnc.mnc_data[i] = data_ptr[offset++];
  }

  if(mcc_mnc_ptr->mnc.mnc_data[i-1] == 'F')
  {
    mcc_mnc_ptr->mnc.mnc_len = 2;
    mcc_mnc_ptr->mnc.mnc_data[i-1] = '\0';
  }

  return SIMLOCK_SUCCESS;
}/* simlock_remote_lock_convert_mcc_mnc */


/*===========================================================================
FUNCTION SIMLOCK_REMOTE_LOCK_READ_TIME_FILE

DESCRIPTION
  This function reads the remote lock time file and provides the
  last times data

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_remote_lock_read_time_file
(
  simlock_remote_lock_timestamp_type    * time_data_ptr
)
{
  simlock_result_enum_type    simlock_status     = SIMLOCK_SUCCESS;
  uint32                      time_file_size     = 0;
  uint8                     * time_file_data_ptr = NULL;

  if(time_data_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  do
  {
    simlock_status = simlock_file_read(&time_file_data_ptr,
                                       &time_file_size,
                                       SIMLOCK_FILE_REMOTE_BLOB_TIME,
                                       SIMLOCK_SFS_LOCAL);
    if((simlock_status != SIMLOCK_SUCCESS) ||
       (time_file_data_ptr == NULL))
    {
      simlock_status = SIMLOCK_GENERIC_ERROR;
      break;
    }

     /* if the file size is not of the expected size,
       then it is not valid */
    if(time_file_size != sizeof(simlock_remote_lock_timestamp_type))
    {
      simlock_status = SIMLOCK_GENERIC_ERROR;
      break;
    }

    simlock_memscpy((uint8 *)time_data_ptr,
                    sizeof(simlock_remote_lock_timestamp_type),
                    time_file_data_ptr,
                    time_file_size);

    /* File version check */
    if(time_data_ptr->version != SIMLOCK_REMOTE_LOCK_TIME_FILE_VERSION)
    {
      simlock_status = SIMLOCK_GENERIC_ERROR;
      break;
    }
  }while (0);

  SIMLOCK_MEM_FREE(time_file_data_ptr);
  return simlock_status;
} /* simlock_remote_lock_read_time_file */


/*===========================================================================
FUNCTION SIMLOCK_REMOTE_LOCK_VALIDATE_AND_STORE_TIMESTAMP

DESCRIPTION
  This function validates the timestamp contained in the blob request
  If it has a valid time stamp, it writes to the sfs

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_remote_lock_validate_and_store_timestamp(
  uint64 timestamp
)
{
  simlock_remote_lock_timestamp_type  time_data;
  simlock_result_enum_type            simlock_status = SIMLOCK_SUCCESS;

  memset(&time_data, 0x00, sizeof(time_data));

  simlock_status = simlock_remote_lock_read_time_file(&time_data);

  /* If the read fails, it would be the first request case */
  if((simlock_status != SIMLOCK_SUCCESS) ||
     (time_data.timestamp < timestamp))
  {
    time_data.version = SIMLOCK_REMOTE_LOCK_TIME_FILE_VERSION;
    time_data.timestamp = timestamp;

    simlock_status =  simlock_file_write((uint8 *)&time_data,
                                         sizeof(simlock_remote_lock_timestamp_type),
                                         SIMLOCK_FILE_REMOTE_BLOB_TIME,
                                         SIMLOCK_SFS_LOCAL);
  }
  else
  {
    SIMLOCK_MSG_MED_2("Last time stamp is 0x%x 0x%x",
                       time_data.timestamp >> 32 & 0xFFFFFFFF,
                       time_data.timestamp & 0xFFFFFFFF);
    simlock_status = SIMLOCK_GENERIC_ERROR;
  }

  return simlock_status;
}/* simlock_remote_lock_validate_and_store_timestamp */
#endif /* FEATURE_SIMLOCK_QC_REMOTE_LOCK */


#ifdef FEATURE_SIMLOCK_RSU
/*===========================================================================
FUNCTION SIMLOCK_RSU_CONFIG_POPULATE_CODE_DATA

DESCRIPTION
  This function populates the lock code data by converting
  the lock data given in request.
  Freeing memory allocated in this function is caller's responsibility.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_rsu_config_populate_code_data(
  simlock_rsu_config_mcc_mnc_list_type   mcc_mnc_info,
  simlock_rsu_config_gid_list_type       gid_info,
  simlock_rsu_config_data_type          *config_data_ptr)
{
  simlock_result_enum_type        simlock_status    = SIMLOCK_SUCCESS;
  uint16                          code_cnt          = 0;
  uint16                          gid_code_cnt      = 0;
  uint16                          num_nw_lock_codes = 0;
  uint16                          num_sp_lock_codes = 0;
  uint8                           nw_code_cnt       = 0;
  uint8                           sp_code_cnt       = 0;
  simlock_nw_code_data_type       nw_code_data;

  if (config_data_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  /* Populate NW Lock 
     Number of possible network lock codes is the sum of
     a. Default MCC MNCs present in simlock_rsu_always_present_codes
     b. Number of MCC MNCs given in the request */
  num_nw_lock_codes = (sizeof(simlock_rsu_always_present_codes)/sizeof(simlock_rsu_config_mcc_mnc_type)) +
                       mcc_mnc_info.num_mcc_mnc_codes;

  if(num_nw_lock_codes > 0)
  {
    SIMLOCK_CHECK_AND_MEM_MALLOC(config_data_ptr->code_data.lock_code_data.nw_lock.nw_code_ptr,
                                 num_nw_lock_codes * sizeof(simlock_nw_code_data_type));
    if (config_data_ptr->code_data.lock_code_data.nw_lock.nw_code_ptr == NULL)
    {
      return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
  }

  /* Add the always present codes */
  for(code_cnt = 0; code_cnt < (sizeof(simlock_rsu_always_present_codes)/sizeof(simlock_rsu_config_mcc_mnc_type)) &&
                    nw_code_cnt < SIMLOCK_NUM_CODES_MAX; code_cnt++, nw_code_cnt++)
  {
    memset(&nw_code_data, 0x00, sizeof(simlock_nw_code_data_type));

    simlock_status = simlock_rsu_config_convert_mcc_mnc(simlock_rsu_always_present_codes[code_cnt],
                                                        &nw_code_data);
    if(simlock_status == SIMLOCK_SUCCESS)
    {
      config_data_ptr->code_data.lock_code_data.nw_lock.nw_codes++;
      config_data_ptr->code_data.lock_code_data.nw_lock.nw_code_ptr[nw_code_cnt] = nw_code_data;
    }
  }

  /* Add the codes given in the request */
  for(code_cnt = 0; (code_cnt < mcc_mnc_info.num_mcc_mnc_codes &&
                     nw_code_cnt < SIMLOCK_NUM_CODES_MAX &&
                     mcc_mnc_info.mcc_mnc_code_ptr != NULL); code_cnt++, nw_code_cnt++)
  {
    if(simlock_rsu_config_is_mcc_mnc_in_always_present_list(mcc_mnc_info.mcc_mnc_code_ptr[code_cnt]))
    {
      continue;
    }

    memset(&nw_code_data, 0x00, sizeof(simlock_nw_code_data_type));

    simlock_status = simlock_rsu_config_convert_mcc_mnc(mcc_mnc_info.mcc_mnc_code_ptr[code_cnt],
                                                        &nw_code_data);

    if(simlock_status == SIMLOCK_SUCCESS)
    {
      config_data_ptr->code_data.lock_code_data.nw_lock.nw_codes++;
      config_data_ptr->code_data.lock_code_data.nw_lock.nw_code_ptr[nw_code_cnt] = nw_code_data;
    }
  }

  /* Populate SP Lock 
     Number of possible SP lock codes is the product of
     a. num_nw_lock_codes (calculated above)
     b. Number of GIDs given in the request */
  num_sp_lock_codes = num_nw_lock_codes * gid_info.num_gid_codes;
  if(num_sp_lock_codes > 0)
  {
    SIMLOCK_CHECK_AND_MEM_MALLOC(config_data_ptr->code_data.lock_code_data.sp_lock.sp_code_ptr,
                                 num_sp_lock_codes * sizeof(simlock_sp_code_data_type));
    if (config_data_ptr->code_data.lock_code_data.sp_lock.sp_code_ptr == NULL)
    {
      return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
  }

  /* Add the SP lock codes with always present codes MCC MNCs with GIDs given in request */
  for(code_cnt = 0; code_cnt < (sizeof(simlock_rsu_always_present_codes)/sizeof(simlock_rsu_config_mcc_mnc_type));
      code_cnt++)
  {
    memset(&nw_code_data, 0x00, sizeof(simlock_nw_code_data_type));

    simlock_status = simlock_rsu_config_convert_mcc_mnc(simlock_rsu_always_present_codes[code_cnt],
                                                        &nw_code_data);
    if(simlock_status == SIMLOCK_SUCCESS)
    {
      for(gid_code_cnt = 0; (gid_code_cnt < gid_info.num_gid_codes &&
                             sp_code_cnt < SIMLOCK_NUM_CODES_MAX &&
                             gid_info.gid_code_ptr != NULL); gid_code_cnt++, sp_code_cnt++)
      {
        config_data_ptr->code_data.lock_code_data.sp_lock.sp_codes++;
        config_data_ptr->code_data.lock_code_data.sp_lock.sp_code_ptr[sp_code_cnt].nw_code = nw_code_data;
        simlock_status = simlock_rsu_config_convert_gid(gid_info.gid_code_ptr[gid_code_cnt],
                                                        &config_data_ptr->code_data.lock_code_data.sp_lock.sp_code_ptr[sp_code_cnt].gid1);
        if(simlock_status != SIMLOCK_SUCCESS)
        {
          return simlock_status;
        }
      } /* end loop for GIDs */
    }
  } /* end loop for MCC MNCs */

  /* Add the SP lock codes with MCC MNCs and GIDs given in request */
  for(code_cnt = 0; (code_cnt < mcc_mnc_info.num_mcc_mnc_codes &&
                     mcc_mnc_info.mcc_mnc_code_ptr != NULL); code_cnt++)
  {
    if(simlock_rsu_config_is_mcc_mnc_in_always_present_list(mcc_mnc_info.mcc_mnc_code_ptr[code_cnt]))
    {
      continue;
    }

    memset(&nw_code_data, 0x00, sizeof(simlock_nw_code_data_type));

    simlock_status = simlock_rsu_config_convert_mcc_mnc(mcc_mnc_info.mcc_mnc_code_ptr[code_cnt],
                                                        &nw_code_data);
    if(simlock_status == SIMLOCK_SUCCESS)
    {
      for(gid_code_cnt = 0; (gid_code_cnt < gid_info.num_gid_codes &&
                             sp_code_cnt < SIMLOCK_NUM_CODES_MAX &&
                             gid_info.gid_code_ptr != NULL) ; gid_code_cnt++, sp_code_cnt++)
      {
        config_data_ptr->code_data.lock_code_data.sp_lock.sp_codes++;
        config_data_ptr->code_data.lock_code_data.sp_lock.sp_code_ptr[sp_code_cnt].nw_code = nw_code_data;
        simlock_status = simlock_rsu_config_convert_gid(gid_info.gid_code_ptr[gid_code_cnt],
                                                        &config_data_ptr->code_data.lock_code_data.sp_lock.sp_code_ptr[sp_code_cnt].gid1);
        if(simlock_status != SIMLOCK_SUCCESS)
        {
          return simlock_status;
        }
      } /* end loop for GIDs */
    }
  } /* end loop for MCC MNCs */

  return SIMLOCK_SUCCESS;
} /* simlock_rsu_config_populate_code_data */
#endif /* FEATURE_SIMLOCK_RSU */


/*===========================================================================
FUNCTION SIMLOCK_RSU_CONFIG_PARSE_DATA

DESCRIPTION
  This function sets the simlock configuration data received in the request

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_rsu_config_parse_data
(
  uint32                             in_data_len,
  const uint8                     *  in_data_ptr,
  simlock_rsu_config_data_type    *  out_config_data_ptr
)
{
  simlock_result_enum_type               simlock_status  = SIMLOCK_SUCCESS;
  uint16                                 offset          = 0;
  uint16                                 code_index      = 0;
  uint16                                 config_data_len = 0;
#ifdef FEATURE_SIMLOCK_RSU
  simlock_rsu_config_mcc_mnc_type        mcc_mnc         = 0;
  simlock_rsu_config_gid_type            gid             = 0;
  uint16                                 num_of_codes    = 0;
  simlock_rsu_config_mcc_mnc_list_type   mcc_mnc_info    = {0, NULL};
  simlock_rsu_config_gid_list_type       gid_info        = {0, NULL};

  PACKED struct PACKED_POST
  {
    uint16   version_major;
    uint16   version_minor;
    uint8    protection_type;
    uint8    simlock_state;
    uint16   blob_len;
  } top_data;

  PACKED struct PACKED_POST
  {
    uint8    rfu[SIMLOCK_RSU_CONFIG_RFU_SIZE];
    uint64   start_time_ms;
    uint32   end_time_s;
    uint8    imei[SIMLOCK_RSU_CONFIG_IMEI_LEN];
  } bottom_data;
#elif defined(FEATURE_SIMLOCK_QC_REMOTE_LOCK)
  simlock_nw_code_data_type              mcc_mnc_data;

  PACKED struct PACKED_POST
  {
    uint8   version_major;
    uint8   version_minor;
    uint8   protection_type;
    uint8   simlock_state;
    uint8   imei[SIMLOCK_IMEI_MAX];
    uint64  time_stamp;
    uint16  blob_len;
  } blob_header_data;
#endif /* FEATURE_SIMLOCK_RSU || FEATURE_SIMLOCK_QC_REMOTE_LOCK */

  if((in_data_ptr == NULL) ||
     (in_data_len == 0)    ||
     (out_config_data_ptr == NULL))
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

#ifdef FEATURE_SIMLOCK_RSU
  memset(&top_data, 0x00, sizeof(top_data));
  memset(&bottom_data, 0x00, sizeof(bottom_data));

  /* if the data len is less than the top and bottome data size,
     then it is not valid */
  if(in_data_len < (sizeof(top_data) + sizeof(bottom_data)))
  {
    SIMLOCK_MSG_ERR_1("RSU config data length is less than minimum size, 0x%x",
                      in_data_len);
    return SIMLOCK_GENERIC_ERROR;
  }

  /* copy the config top data to packed struct */
  simlock_memscpy((uint8 *)&top_data,
                  sizeof(top_data),
                  in_data_ptr,
                  sizeof(top_data));

  out_config_data_ptr->header_data.version_major = SIMLOCK_RSU_SWAP_BYTE_ORDER_16(top_data.version_major);
  out_config_data_ptr->header_data.version_minor = SIMLOCK_RSU_SWAP_BYTE_ORDER_16(top_data.version_minor);

  /* check the versions */
  if((out_config_data_ptr->header_data.version_major != SIMLOCK_RSU_CONFIG_MAJOR_VERSION_ONE) ||
     (out_config_data_ptr->header_data.version_minor != SIMLOCK_RSU_CONFIG_MINOR_VERSION_MIN))
  {
    SIMLOCK_MSG_ERR_0("RSU blob version check failed");
    return SIMLOCK_UNSUPPORTED;
  }

  simlock_status = simlock_rsu_config_parse_protection_type(top_data.protection_type,
                                                            &out_config_data_ptr->header_data.protection_type);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    SIMLOCK_MSG_ERR_1("failed to parse RSU protection type 0x%x",
                      top_data.protection_type);
    return simlock_status;
  }

  simlock_status = simlock_rsu_config_parse_lock_state(top_data.simlock_state,
                                                       &out_config_data_ptr->header_data.lock_state);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    SIMLOCK_MSG_ERR_1("failed to parse RSU lock state 0x%x",
                      top_data.simlock_state);
    return simlock_status;
  }

  config_data_len = SIMLOCK_RSU_SWAP_BYTE_ORDER_16(top_data.blob_len);

  /* the config data len plus the top data len is 
     less than than total data size, it is an error */
  if((config_data_len + sizeof(top_data)) < in_data_len)
  {
    SIMLOCK_MSG_ERR_2("RSU config data lengths inconsistent, Total len:0x%x, Data len: 0x%x",
                       in_data_len, config_data_len);
    return SIMLOCK_GENERIC_ERROR;
  }

  offset = sizeof(top_data);

  do
  {
    if (offset + sizeof(num_of_codes) > in_data_len)
    {
      simlock_status = SIMLOCK_GENERIC_ERROR;
      break;
    }

    /* get the number of mcc-mnc */
    simlock_memscpy((uint8 *)&num_of_codes,
                    sizeof(num_of_codes),
                    &in_data_ptr[offset],
                    sizeof(num_of_codes));

    mcc_mnc_info.num_mcc_mnc_codes = SIMLOCK_RSU_SWAP_BYTE_ORDER_16(num_of_codes);

    SIMLOCK_MSG_MED_1("Num MCC-MNCs - 0x%x",
                       mcc_mnc_info.num_mcc_mnc_codes);

    if(mcc_mnc_info.num_mcc_mnc_codes > 0)
    {
      /* caller is responsible to FREE this memory */
      SIMLOCK_CHECK_AND_MEM_MALLOC(mcc_mnc_info.mcc_mnc_code_ptr,
                                   (mcc_mnc_info.num_mcc_mnc_codes * sizeof(simlock_rsu_config_mcc_mnc_type)));
      if(mcc_mnc_info.mcc_mnc_code_ptr == NULL)
      {
        simlock_status = SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
        break;
      }
    }

    offset += sizeof(num_of_codes);

    for(code_index = 0; code_index < mcc_mnc_info.num_mcc_mnc_codes; code_index++)
    {
      if(offset + sizeof(mcc_mnc) > in_data_len)
      {
        simlock_status = SIMLOCK_GENERIC_ERROR;
        break;
      }

      /* get the mcc-mnc */
      simlock_memscpy((uint8 *)&mcc_mnc,
                      sizeof(mcc_mnc),
                      &in_data_ptr[offset],
                      sizeof(mcc_mnc));

      mcc_mnc_info.mcc_mnc_code_ptr[code_index] = SIMLOCK_RSU_SWAP_BYTE_ORDER_32(mcc_mnc);
      offset += sizeof(mcc_mnc);
    }

    if (code_index != mcc_mnc_info.num_mcc_mnc_codes)
    {
      break;
    }

    if(offset + sizeof(num_of_codes) > in_data_len)
    {
      simlock_status = SIMLOCK_GENERIC_ERROR;
      break;
    }

    /* get the number of gids */
    simlock_memscpy((uint8 *)&num_of_codes,
                     sizeof(num_of_codes),
                     &in_data_ptr[offset],
                      sizeof(num_of_codes));

    gid_info.num_gid_codes = SIMLOCK_RSU_SWAP_BYTE_ORDER_16(num_of_codes);

    SIMLOCK_MSG_MED_1("Num GIDs - 0x%x",
                      gid_info.num_gid_codes);

    if(gid_info.num_gid_codes > 0)
    {
      /* caller is responsible to FREE this memory */
      SIMLOCK_CHECK_AND_MEM_MALLOC(gid_info.gid_code_ptr,
                                   (gid_info.num_gid_codes * sizeof(simlock_rsu_config_gid_type)));
      if(gid_info.gid_code_ptr == NULL)
      {
        simlock_status = SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
      }
    }

    offset += sizeof(num_of_codes);

    for(code_index = 0;
        code_index < gid_info.num_gid_codes;
        code_index++)
    {
      if(offset + sizeof(gid) > in_data_len)
      {
        simlock_status = SIMLOCK_GENERIC_ERROR;
        break;
      }

      /* get the gid */
      simlock_memscpy((uint8 *)&gid,
                      sizeof(gid),
                      &in_data_ptr[offset],
                      sizeof(gid));
      gid_info.gid_code_ptr[code_index] = SIMLOCK_RSU_SWAP_BYTE_ORDER_16(gid);
      offset += sizeof(gid);
    }

    if (code_index != gid_info.num_gid_codes)
    {
      break;
    }

    simlock_status = simlock_rsu_config_populate_code_data(mcc_mnc_info, gid_info, out_config_data_ptr);
  }while(0);

  SIMLOCK_MEM_FREE(mcc_mnc_info.mcc_mnc_code_ptr);
  SIMLOCK_MEM_FREE(gid_info.gid_code_ptr);

  if (simlock_status != SIMLOCK_SUCCESS)
  {
    return simlock_status;
  }

  if(offset + sizeof(bottom_data) > in_data_len)
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  /* copy the bottom config data to packed struct */
  simlock_memscpy((uint8 *)&bottom_data,
                  sizeof(bottom_data),
                  &in_data_ptr[offset],
                  sizeof(bottom_data));

  out_config_data_ptr->code_data.start_time = (SIMLOCK_RSU_SWAP_BYTE_ORDER_64(bottom_data.start_time_ms) / 1000);
  out_config_data_ptr->code_data.end_time   = SIMLOCK_RSU_SWAP_BYTE_ORDER_32(bottom_data.end_time_s);

  out_config_data_ptr->imei.imei_len = SIMLOCK_IMEI_MAX;

  /* imei in the config is 16-bytes and is zero padded
     in the end. We ignore the last byte */
  simlock_memscpy(out_config_data_ptr->imei.imei_data,
                  out_config_data_ptr->imei.imei_len,
                  bottom_data.imei,
                  out_config_data_ptr->imei.imei_len);

  offset += sizeof(bottom_data);

  if(out_config_data_ptr->header_data.protection_type == SIMLOCK_RSU_CONFIG_PROTECTION_SHA256)
  {
    if(offset + sizeof(out_config_data_ptr->auth_data.data.hmac) > in_data_len)
    {
      return SIMLOCK_GENERIC_ERROR;
    }

   /* copy the hmac data */
    simlock_memscpy(out_config_data_ptr->auth_data.data.hmac,
                    sizeof(out_config_data_ptr->auth_data.data.hmac),
                    &in_data_ptr[offset],
                    sizeof(out_config_data_ptr->auth_data.data.hmac));

    /* the msg to autheniticate is the entire data except the HMAC
       in the data */
    out_config_data_ptr->auth_data.msg_len = (in_data_len - SIMLOCK_HMAC_SHA256_LEN);

    /* caller is responsible to FREE this memory */
    SIMLOCK_CHECK_AND_MEM_MALLOC(out_config_data_ptr->auth_data.msg_data_ptr,
                                 out_config_data_ptr->auth_data.msg_len);
    if(out_config_data_ptr->auth_data.msg_data_ptr == NULL)
    {
      return SIMLOCK_GENERIC_ERROR;
    }

    /* copy the data without the trailing mac This msg data
       will be required in the HMAC verification */
    simlock_memscpy(out_config_data_ptr->auth_data.msg_data_ptr,
                    out_config_data_ptr->auth_data.msg_len,
                    in_data_ptr,
                    out_config_data_ptr->auth_data.msg_len);
  }
#elif defined(FEATURE_SIMLOCK_QC_REMOTE_LOCK)
  memset(&blob_header_data, 0x00, sizeof(blob_header_data));

  if(in_data_len < (sizeof(blob_header_data) + SIMLOCK_REMOTE_LOCK_MIN_CODE_DATA_LEN))
  {
    SIMLOCK_MSG_ERR_1("Remote Lock config data length is less than minimum size, 0x%x",
                      in_data_len);
    return SIMLOCK_GENERIC_ERROR;
  }

  simlock_memscpy((uint8 *)&blob_header_data,
                  sizeof(blob_header_data),
                  in_data_ptr,
                  sizeof(blob_header_data));

  out_config_data_ptr->header_data.version_major = blob_header_data.version_major;
  out_config_data_ptr->header_data.version_minor = blob_header_data.version_minor;

  /* check the versions */
  if((out_config_data_ptr->header_data.version_major != SIMLOCK_REMOTE_LOCK_CONFIG_MAJOR_VERSION) ||
     (out_config_data_ptr->header_data.version_minor != SIMLOCK_REMOTE_LOCK_CONFIG_MINOR_VERSION))
  {
    SIMLOCK_MSG_ERR_0("Remote Lock blob version check failed");
    return SIMLOCK_UNSUPPORTED;
  }

  simlock_status = simlock_rsu_config_parse_protection_type(blob_header_data.protection_type,
                                                            &out_config_data_ptr->header_data.protection_type);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    SIMLOCK_MSG_ERR_1("Failed to parse Remote Lock protection type 0x%x",
                       blob_header_data.protection_type);
    return simlock_status;
  }

  simlock_status = simlock_rsu_config_parse_lock_state(blob_header_data.simlock_state,
                                                       &out_config_data_ptr->header_data.lock_state);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    SIMLOCK_MSG_ERR_1("Failed to parse lock state 0x%x",
                       blob_header_data.simlock_state);
    return simlock_status;
  }

  out_config_data_ptr->imei.imei_len = SIMLOCK_IMEI_MAX;
  simlock_memscpy(out_config_data_ptr->imei.imei_data,
                  out_config_data_ptr->imei.imei_len,
                  blob_header_data.imei,
                  out_config_data_ptr->imei.imei_len);

  out_config_data_ptr->header_data.time_stamp = SIMLOCK_RSU_SWAP_BYTE_ORDER_64(blob_header_data.time_stamp);

  config_data_len = SIMLOCK_RSU_SWAP_BYTE_ORDER_16(blob_header_data.blob_len);

  if((config_data_len + sizeof(blob_header_data)) < in_data_len)
  {
    SIMLOCK_MSG_ERR_2("Remote Lock config data lengths inconsistent, Total len:0x%x, Data len: 0x%x",
                       in_data_len, config_data_len);
    return SIMLOCK_GENERIC_ERROR;
  }

  offset = sizeof(blob_header_data);

  /* Parse NW lock data */
  if (offset + SIMLOCK_REMOTE_LOCK_NUM_CODES_LENGTH > in_data_len)
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  out_config_data_ptr->code_data.lock_code_data.nw_lock.nw_codes = in_data_ptr[offset];

  if(out_config_data_ptr->code_data.lock_code_data.nw_lock.nw_codes > 0)
  {
    /* caller is responsible to FREE this memory */
    SIMLOCK_CHECK_AND_MEM_MALLOC(out_config_data_ptr->code_data.lock_code_data.nw_lock.nw_code_ptr,
                                 (out_config_data_ptr->code_data.lock_code_data.nw_lock.nw_codes * sizeof(simlock_nw_code_data_type)));
    if(out_config_data_ptr->code_data.lock_code_data.nw_lock.nw_code_ptr == NULL)
    {
      return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
  }

  offset += SIMLOCK_REMOTE_LOCK_NUM_CODES_LENGTH;

  for(code_index = 0;
      code_index < out_config_data_ptr->code_data.lock_code_data.nw_lock.nw_codes;
      code_index++)
  {
    if(offset + SIMLOCK_REMOTE_LOCK_NW_CODE_DATA_SIZE > in_data_len)
    {
      return SIMLOCK_GENERIC_ERROR;
    }

    memset(&mcc_mnc_data, 0x00, sizeof(mcc_mnc_data));
    if(SIMLOCK_SUCCESS == simlock_remote_lock_convert_mcc_mnc(&in_data_ptr[offset],
                                                              (in_data_len - offset),
                                                              &mcc_mnc_data))
    {
      memscpy(&out_config_data_ptr->code_data.lock_code_data.nw_lock.nw_code_ptr[code_index],
              sizeof(out_config_data_ptr->code_data.lock_code_data.nw_lock.nw_code_ptr[code_index]),
              &mcc_mnc_data,
              sizeof(mcc_mnc_data));
    }
    offset += SIMLOCK_REMOTE_LOCK_NW_CODE_DATA_SIZE;
  }

  /* Parse NS lock data */
  if (offset + SIMLOCK_REMOTE_LOCK_NUM_CODES_LENGTH > in_data_len)
  {
    return SIMLOCK_GENERIC_ERROR;
  }
  out_config_data_ptr->code_data.lock_code_data.ns_lock.ns_codes = in_data_ptr[offset];

  if(out_config_data_ptr->code_data.lock_code_data.ns_lock.ns_codes > 0)
  {
    /* caller is responsible to FREE this memory */
    SIMLOCK_CHECK_AND_MEM_MALLOC(out_config_data_ptr->code_data.lock_code_data.ns_lock.ns_code_ptr,
                                 (out_config_data_ptr->code_data.lock_code_data.ns_lock.ns_codes * sizeof(simlock_ns_code_data_type)));
    if(out_config_data_ptr->code_data.lock_code_data.ns_lock.ns_code_ptr == NULL)
    {
      return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
  }

  offset += SIMLOCK_REMOTE_LOCK_NUM_CODES_LENGTH;

  for(code_index = 0;
      code_index < out_config_data_ptr->code_data.lock_code_data.ns_lock.ns_codes;
      code_index++)
  {
    if(offset + SIMLOCK_REMOTE_LOCK_NW_CODE_DATA_SIZE > in_data_len)
    {
      return SIMLOCK_GENERIC_ERROR;
    }

    memset(&mcc_mnc_data, 0x00, sizeof(mcc_mnc_data));
    if(SIMLOCK_SUCCESS == simlock_remote_lock_convert_mcc_mnc(&in_data_ptr[offset],
                                                              (in_data_len - offset),
                                                              &mcc_mnc_data))
    {
      memscpy(&out_config_data_ptr->code_data.lock_code_data.ns_lock.ns_code_ptr[code_index].nw_code,
              sizeof(out_config_data_ptr->code_data.lock_code_data.ns_lock.ns_code_ptr[code_index].nw_code),
              &mcc_mnc_data,
              sizeof(mcc_mnc_data));
    }

    offset += SIMLOCK_REMOTE_LOCK_NW_CODE_DATA_SIZE;

    /* Copy the IMSI 6th and 7th byte values */
    out_config_data_ptr->code_data.lock_code_data.ns_lock.ns_code_ptr[code_index].imsi_digit6 = in_data_ptr[offset++];
    out_config_data_ptr->code_data.lock_code_data.ns_lock.ns_code_ptr[code_index].imsi_digit7 = in_data_ptr[offset++];
  }

  /* Parse SP lock data */
  if (offset + SIMLOCK_REMOTE_LOCK_NUM_CODES_LENGTH > in_data_len)
  {
    return SIMLOCK_GENERIC_ERROR;
  }
  out_config_data_ptr->code_data.lock_code_data.sp_lock.sp_codes = in_data_ptr[offset];

  if(out_config_data_ptr->code_data.lock_code_data.sp_lock.sp_codes > 0)
  {
    /* caller is responsible to FREE this memory */
    SIMLOCK_CHECK_AND_MEM_MALLOC(out_config_data_ptr->code_data.lock_code_data.sp_lock.sp_code_ptr,
                                 (out_config_data_ptr->code_data.lock_code_data.sp_lock.sp_codes * sizeof(simlock_sp_code_data_type)));
    if(out_config_data_ptr->code_data.lock_code_data.sp_lock.sp_code_ptr == NULL)
    {
      return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
  }

  offset += SIMLOCK_REMOTE_LOCK_NUM_CODES_LENGTH;

  for(code_index = 0;
      code_index < out_config_data_ptr->code_data.lock_code_data.sp_lock.sp_codes;
      code_index++)
  {
    if(offset + SIMLOCK_REMOTE_LOCK_NW_CODE_DATA_SIZE > in_data_len)
    {
      return SIMLOCK_GENERIC_ERROR;
    }

    memset(&mcc_mnc_data, 0x00, sizeof(mcc_mnc_data));
    if(SIMLOCK_SUCCESS == simlock_remote_lock_convert_mcc_mnc(&in_data_ptr[offset],
                                                              (in_data_len - offset),
                                                              &mcc_mnc_data))
    {
      memscpy(&out_config_data_ptr->code_data.lock_code_data.sp_lock.sp_code_ptr[code_index].nw_code,
              sizeof(out_config_data_ptr->code_data.lock_code_data.sp_lock.sp_code_ptr[code_index].nw_code),
              &mcc_mnc_data,
              sizeof(mcc_mnc_data));
    }

    offset += SIMLOCK_REMOTE_LOCK_NW_CODE_DATA_SIZE;

    /* Copy the GID value */
    out_config_data_ptr->code_data.lock_code_data.sp_lock.sp_code_ptr[code_index].gid1.gid_len = 1;
    out_config_data_ptr->code_data.lock_code_data.sp_lock.sp_code_ptr[code_index].gid1.gid_data[0] = in_data_ptr[offset++];
  }

  if(out_config_data_ptr->header_data.protection_type == SIMLOCK_RSU_CONFIG_PROTECTION_RSA)
  {
    if(offset + sizeof(out_config_data_ptr->auth_data.data.rsa) > in_data_len)
    {
      return SIMLOCK_GENERIC_ERROR;
    }

   /* copy the rsa data */
    simlock_memscpy(out_config_data_ptr->auth_data.data.rsa,
                    sizeof(out_config_data_ptr->auth_data.data.rsa),
                    &in_data_ptr[offset],
                    sizeof(out_config_data_ptr->auth_data.data.rsa));

    /* the msg to autheniticate is the entire data except the rsa
       in the data */
    out_config_data_ptr->auth_data.msg_len = (in_data_len - SIMLOCK_RSA_LEN);

    /* caller is responsible to FREE this memory */
    SIMLOCK_CHECK_AND_MEM_MALLOC(out_config_data_ptr->auth_data.msg_data_ptr,
                                 out_config_data_ptr->auth_data.msg_len);
    if(out_config_data_ptr->auth_data.msg_data_ptr == NULL)
    {
      return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
    }

    /* copy the data without the trailing mac This msg data
       will be required in the RSA verification */
    simlock_memscpy(out_config_data_ptr->auth_data.msg_data_ptr,
                    out_config_data_ptr->auth_data.msg_len,
                    in_data_ptr,
                    out_config_data_ptr->auth_data.msg_len);
  }
#endif /* FEATURE_SIMLOCK_RSU || FEATURE_SIMLOCK_QC_REMOTE_LOCK */

  return SIMLOCK_SUCCESS;
} /* simlock_rsu_config_parse_data */


/*===========================================================================
FUNCTION SIMLOCK_RSU_CONFIG_VALIDATE_BLOB

DESCRIPTION
  This function validates the simlock blob received

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type  simlock_rsu_config_validate_blob(
  simlock_rsu_config_data_type  *config_data_ptr
  )
{
  simlock_result_enum_type   simlock_status = SIMLOCK_GENERIC_ERROR;

  if (config_data_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  /* For commercial deployment, only the protection type SHA256 is supported.
     This can be supported also in test environment */
  /* verify the integrity of the data */
  simlock_status = simlock_rsu_config_verify_data_integrity(config_data_ptr);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    SIMLOCK_MSG_ERR_0("data integrity check failed");
    return simlock_status;
  }

  /* check the IMEI */
  simlock_status = simlock_rsu_config_check_imei(config_data_ptr->imei);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    SIMLOCK_MSG_ERR_0("imei check failed");
    return simlock_status;
  }

#ifdef FEATURE_SIMLOCK_QC_REMOTE_LOCK
  /* Validate and store the time stamp */
  simlock_status = simlock_remote_lock_validate_and_store_timestamp(config_data_ptr->header_data.time_stamp);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    return simlock_status;
  }
#endif /* FEATURE_SIMLOCK_QC_REMOTE_LOCK */

  return SIMLOCK_SUCCESS;
} /* simlock_rsu_config_validate_blob */


/*===========================================================================
FUNCTION SIMLOCK_RSU_CONFIG_SET_DATA

DESCRIPTION
  This function sets the simlock configuration data received in the request

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_rsu_config_set_data
(
  uint32           raw_data_len,
  const uint8   *  raw_data_ptr
)
{
  simlock_result_enum_type             simlock_status          = SIMLOCK_SUCCESS;
  simlock_rsu_config_data_type       * decoded_config_data_ptr = NULL;

  if((raw_data_len == 0) ||
     (raw_data_ptr == NULL))
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  SIMLOCK_CHECK_AND_MEM_MALLOC(decoded_config_data_ptr,
                               sizeof(simlock_rsu_config_data_type));
  if(decoded_config_data_ptr == NULL)
  {
    return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* parse the data */
  simlock_status = simlock_rsu_config_parse_data(raw_data_len,
                                                 raw_data_ptr,
                                                 decoded_config_data_ptr);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    SIMLOCK_MSG_ERR_0("parse config data failed");
    simlock_rsu_config_free_code_data(decoded_config_data_ptr);
    return simlock_status;
  }

  /* validate the blob request */
  simlock_status = simlock_rsu_config_validate_blob(decoded_config_data_ptr);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    simlock_rsu_config_free_code_data(decoded_config_data_ptr);
    return simlock_status;
  }

  /* apply the configuration */
  simlock_status = simlock_rsu_config_apply_configuration(decoded_config_data_ptr);

  simlock_rsu_config_free_code_data(decoded_config_data_ptr);
  return simlock_status;
} /* simlock_rsu_config_set_data */


/*===========================================================================
FUNCTION SIMLOCK_RSU_CONFIG_IS_LOCK_ALLOWED

DESCRIPTION
  This function checks if the lock request is to be allowed or not.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  boolean

SIDE EFFECTS
  NONE
===========================================================================*/
boolean simlock_rsu_config_is_lock_allowed(
  const simlock_message_request_data_type *req_data_ptr
)
{
  boolean                     is_allowed           = FALSE;
#ifdef FEATURE_SIMLOCK_RSU
  uint8                       i                    = 0;
  uint8                       k                    = 0;
  simlock_nw_code_data_type  *req_code_data_ptr    = NULL;
  uint8                       num_codes            = 0;
#endif /* FEATURE_SIMLOCK_RSU */

  if(req_data_ptr == NULL)
  {
    return FALSE;
  }

  switch(req_data_ptr->msg_type)
  {
    case SIMLOCK_SET_LOCK_CK_MSG:
      /* Reject the request if following conditions are satisfied :
      1) if request have blacklist
      2) if request have auto lock enabled 
      3) if Ck len > 0. In RSU, categories are locked without Ck.*/
      if((req_data_ptr->message.lock_ck_msg.category.category_type != SIMLOCK_CATEGORY_3GPP_SIM &&
          req_data_ptr->message.lock_ck_msg.category.category_type != SIMLOCK_CATEGORY_3GPP2_RUIM) &&
         (req_data_ptr->message.lock_ck_msg.blacklist ||
          req_data_ptr->message.lock_ck_msg.auto_lock ||
          req_data_ptr->message.lock_ck_msg.ck.simlock_ck_length > 0))
      {
        return FALSE;
      }

#ifdef FEATURE_SIMLOCK_RSU
      switch(req_data_ptr->message.lock_ck_msg.category.category_type)
      {
        case SIMLOCK_CATEGORY_3GPP_NW:
          is_allowed = TRUE;
          num_codes  = req_data_ptr->message.lock_ck_msg.category.code_data.nw_3gpp_code_list.num_nw_codes;

          SIMLOCK_CHECK_AND_MEM_MALLOC(req_code_data_ptr, num_codes * sizeof(simlock_nw_code_data_type));
          if(req_code_data_ptr == NULL)
          {
            return FALSE;
          }

          for(i = 0; i < num_codes; i++)
          {
            req_code_data_ptr[i] = req_data_ptr->message.lock_ck_msg.category.code_data.nw_3gpp_code_list.nw_code_data[i];
          }
          break;

        case SIMLOCK_CATEGORY_3GPP_SP:
          is_allowed = TRUE;
          num_codes  = req_data_ptr->message.lock_ck_msg.category.code_data.sp_3gpp_code_list.num_sp_codes;

          SIMLOCK_CHECK_AND_MEM_MALLOC(req_code_data_ptr, num_codes * sizeof(simlock_nw_code_data_type));
          if(req_code_data_ptr == NULL)
          {
            return FALSE;
          }

          for(i = 0; i < num_codes; i++)
          {
            req_code_data_ptr[i] = req_data_ptr->message.lock_ck_msg.category.code_data.sp_3gpp_code_list.sp_code_data[i].nw_code;
          }
          break;

        case SIMLOCK_CATEGORY_3GPP_SIM:
          is_allowed = TRUE;
          break;

        default:
          is_allowed = FALSE;
          break;
      }
#elif defined(FEATURE_SIMLOCK_QC_REMOTE_LOCK)
      switch(req_data_ptr->message.lock_ck_msg.category.category_type)
      {
        case SIMLOCK_CATEGORY_3GPP_NW:
        case SIMLOCK_CATEGORY_3GPP_NS:
        case SIMLOCK_CATEGORY_3GPP_SP:
        case SIMLOCK_CATEGORY_3GPP_SIM:
          is_allowed = TRUE;
          break;

        default:
          is_allowed = FALSE;
          break;
      }
#endif /* FEATURE_SIMLOCK_QC_REMOTE_LOCK */
      break;

    /* For HCk Lock, Add Lock and Relock messages, Lock request is not allowed */
    case SIMLOCK_SET_LOCK_HCK_MSG:
    case SIMLOCK_ADD_LOCK_CODES_HCK_MSG:
    case SIMLOCK_ADD_LOCK_CODES_CK_MSG:
    case SIMLOCK_RELOCK_DEVICE_MSG:
      is_allowed = FALSE;
      break;

    /* For any other messages, the request should be allowed e.g. Get Status msg, Run algo etc. */
    default:
      is_allowed = TRUE;
      break;
  }

#ifdef FEATURE_SIMLOCK_RSU
  /* If is_allowed found to be TRUE and have a list of code data to be matched then 
     do the comparision. Otherwise, just return.*/
  for(i = 0; ((is_allowed) && (req_code_data_ptr) && (i < num_codes)); i++)
  {
    boolean                     is_match = FALSE;
    simlock_nw_code_data_type   req_mcc_mnc_data;

    /* Copy the next code data in request */
    req_mcc_mnc_data = req_code_data_ptr[i];

    /* Reset is_match flag before comparing next code data */
    is_match         = FALSE;

    for(k = 0;
        k < (sizeof(simlock_rsu_always_present_codes) / sizeof(simlock_rsu_always_present_codes[0]));
        k++)
    {
      simlock_nw_code_data_type   rsu_mcc_mnc_data;

      memset(&rsu_mcc_mnc_data, 0x00, sizeof(rsu_mcc_mnc_data));

      /* Convert the RSU MCC MNC code data to normal format */
      simlock_rsu_config_convert_mcc_mnc(simlock_rsu_always_present_codes[k],
                                         &rsu_mcc_mnc_data);

      /* Check if the current code in the request matches with that of 
      MCC MNC present in simlock_rsu_always_present_codes 
      If matches, then check the next code data in request */
      if(simlock_util_check_if_mcc_matches(rsu_mcc_mnc_data.mcc, req_mcc_mnc_data.mcc) && 
         simlock_util_check_if_mnc_matches(rsu_mcc_mnc_data.mnc, req_mcc_mnc_data.mnc))
      {
        is_match = TRUE;
        break;
      }
    }

    if(!is_match)
    {
      /* Code data in request doesn't match with any of 
      the codes in simlock_rsu_always_present_codes. 
      Hence do not allow the request */
      is_allowed = FALSE;
      break;
    }
  }

  SIMLOCK_MEM_FREE(req_code_data_ptr);
#endif /* FEATURE_SIMLOCK_RSU */

  SIMLOCK_MSG_MED_2("simlock_rsu_config_is_lock_allowed is_allowed 0x%x msg 0x%x", is_allowed, req_data_ptr->msg_type);
  return is_allowed;
}/* simlock_rsu_config_is_lock_allowed */

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

#endif /* FEATURE_SIMLOCK && (FEATURE_SIMLOCK_RSU || FEATURE_SIMLOCK_QC_REMOTE_LOCK) */
