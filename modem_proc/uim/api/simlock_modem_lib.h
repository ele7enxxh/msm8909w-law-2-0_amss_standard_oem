#ifndef SIMLOCK_MODEM_LIB_H
#define SIMLOCK_MODEM_LIB_H

/**
@file simlock_modem_lib.h
@brief This library contains functions for the SIMLock Feature

This interface allows clients to initalize, access and configure
the SIM Lock feature.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The simlock_mainpage.dox file contains all file/group descriptions that are
      in the output PDF generated using Doxygen and Latex. To edit or update
      any of the file/group text in the PDF, edit the mmgsdi_mainpage.dox file or
      contact Tech Pubs.

      The above description for this file is part of the "session_library" group
      description in the simlock_mainpage.dox file.
===========================================================================*/

/*================================================================================

  This file contains Library function that can be used to externally
  initalize, access and configure the SIM Lock feature.

                        COPYRIGHT INFORMATION
Copyright (c) 2014 - 2016 Qualcomm Technologies, Incorporated and its licensors.  All
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     -----------------------------------------------------------
03/09/16   av      Support for autolock whitelist
07/09/15   bcho    Support for whitelist and blacklist codes in same category
07/06/15   bcho    Redundant checks for SIMLOCK
05/15/15   vv      Support for RSU set configuration in simlock
05/15/15   vv      Support for RSU in simlock
03/19/15   at      Introducing new relock feature in SIMLock
09/29/14   vv      Added support for emergency only mode
08/29/14   vv      Added support for temporary unlock
06/04/14   vv      Remove FEATURE_SIMLOCK
06/03/14   vv      Added support for OTA unlock
03/26/14   tl      Add comdef.h
03/14/14   tl      Initial Version


=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "comdef.h"
#include "simlock_common.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/
/* As per the 3GPP spec. 22.022, Annex A.4, the
   max length of OTA unlock request and response is
   defined as below */
#define SIMLOCK_OTA_UNLOCK_DATA_MAX               120
#define SIMLOCK_OTA_UNLOCK_RESP_DATA_LEN          20

/*=============================================================================

                      ABSTRACT DATA TYPE DECLARATIONS

=============================================================================*/
/* This array contains the response status for the OTA unlock request */
typedef uint8  simlock_ota_unlock_resp_type[SIMLOCK_OTA_UNLOCK_RESP_DATA_LEN];

/* ----------------------------------------------------------------------------
   STRUCTURE:      SIMLOCK_OTA_UNLOCK_DATA_TYPE

   DESCRIPTION:
     This structure is for OTA unlock request data
------------------------------------------------------------------------------*/
typedef struct {
  uint8    ota_data_len;
  uint8    ota_data[SIMLOCK_OTA_UNLOCK_DATA_MAX];
} simlock_ota_unlock_data_type;

/* ----------------------------------------------------------------------------
   DATA TYPE:      SIMLOCK_CALLBACK_TYPE
-----------------------------------------------------------------------------*/
/** @brief Client callback function.

    The parameter to the callback function contains the results of the
    procedures performed at the client's request.
*/
typedef void (*simlock_callback_type) (simlock_result_enum_type                    status,
                                       const simlock_message_response_data_type  * resp_ptr,
                                       const void                                * user_data_ptr);

/* ----------------------------------------------------------------------------
   DATA TYPE:      SIMLOCK_OTA_CALLBACK_TYPE
-----------------------------------------------------------------------------*/
/** @brief Client callback function.

    The parameter to the callback function contains the results of the
    procedures performed at the client's request.
*/
typedef void (*simlock_ota_callback_type) (simlock_result_enum_type                    status,
                                           const simlock_ota_unlock_resp_type          resp_data,
                                           const void                                * user_data_ptr);

/* ----------------------------------------------------------------------------
   DATA TYPE:      SIMLOCK_RSU_CALLBACK_TYPE
-----------------------------------------------------------------------------*/
/** @brief Client callback function.

    The parameter to the callback function contains the results of the
    procedures performed at the client's request.
*/
typedef void (*simlock_rsu_callback_type) (simlock_result_enum_type                    status,
                                           const simlock_data_type                     resp_data,
                                           const void                                * user_data_ptr);

/*=============================================================================

                                  FUNCTIONS

=============================================================================*/

/*=============================================================================
FUNCTION        SIMLOCK_SET_POLICY

DESCRIPTION
  This function sets the slot and subscription policy in the SIM Lock feature
  and stores the information in the SFS.

PARAMETERS

RETURN VALUE
  req_data :         Data for setting a SIMLock policy
  simlock_cb_ptr :   Command response call back
  user_data_ptr :    Client's user data

  simlock_result_enum_type:

    SIMLOCK_SUCCESS:                     The command structure was properly
                                         generated and queued onto the SIM Lock
                                         Command Queue.
    SIMLOCK_INCORRECT_PARAMS:            The parameters supplied to the API are not
                                         within appropriate ranges.
    SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED: The sufficient memory is not available to
                                         process the request.
    SIMLOCK_UNSUPPORTED:                 The request is not supported.
    SIMLOCK_GENERIC_ERROR:               An error has occurred while processing
                                         the request.

===============================================================================*/
simlock_result_enum_type simlock_set_policy(
  simlock_set_config_policy_msg_req_type      req_data,
  simlock_callback_type                       simlock_cb_ptr,
  void                                      * user_data_ptr
);

/*=============================================================================
FUNCTION        SIMLOCK_HCK_LOCK

DESCRIPTION
  This function allows the client to provision one SIM Lock feature
  with up to 20 sets of that feature data. The SIM Lock feature
  will then be locked to the device to an HCK which is generated by
  the carrier using a randomly generated SALT key and the original
  control key stored by the carrier.

  When autolock indicator is enabled, the lock data may also contain a valid
  list (whitelist) of lock codes. If that is the case, once the simcard is
  inserted, if the (first) simcard data matches any of the entry in the
  whitelist, the device is autolocked to that simcard.

  In addition, an autolock indicator, blacklist indicator, maximum
  unlock attempt indicator and key generation iteration count indicator
  will be stored along with the security data in the SFS.

PARAMETERS
  req_data :         Data for locking a category
  simlock_cb_ptr :   Command response call back
  user_data_ptr :    Client's user data

RETURN VALUE

  simlock_result_enum_type:

    SIMLOCK_SUCCESS:                     The command structure was properly
                                         generated and queued onto the SIM Lock
                                         Command Queue.
    SIMLOCK_INCORRECT_PARAMS:            The parameters supplied to the API are not
                                         within appropriate ranges.
    SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED: The sufficient memory is not available to
                                         process the request.
    SIMLOCK_UNSUPPORTED:                 The request is not supported.
    SIMLOCK_GENERIC_ERROR:               An error has occurred while processing
                                         the request.

===============================================================================*/
simlock_result_enum_type simlock_hck_lock(
  simlock_set_lock_hck_msg_req_type           req_data,
  simlock_callback_type                       simlock_cb_ptr,
  void                                      * user_data_ptr
);

/*=============================================================================
FUNCTION        SIMLOCK_CK_LOCK

DESCRIPTION
  This function allows the client to provision one SIM Lock feature
  with up to 20 sets of that feature data. The SIM Lock feature will then be
  locked to the device with the CK passed to the client by the carrier.

  When autolock indicator is enabled, the lock data may also contain a valid
  list (whitelist) of lock codes. If that is the case, once the simcard is
  inserted, if the (first) simcard data matches any of the entry in the
  whitelist, the device is autolocked to that simcard.

  In addition, an autolock indicator, blacklist indicator, maximum
  unlock attempt indicator and key generation iteration count indicator
  will be stored along with the security data in the SFS.

PARAMETERS
  req_data :         Data for locking a category
  simlock_cb_ptr :   Command response call back
  user_data_ptr :    Client's user data

RETURN VALUE

  simlock_result_enum_type:

    SIMLOCK_SUCCESS:                     The command structure was properly
                                         generated and queued onto the SIM Lock
                                         Command Queue.
    SIMLOCK_INCORRECT_PARAMS:            The parameters supplied to the API are not
                                         within appropriate ranges.
    SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED: The sufficient memory is not available to
                                         process the request.
    SIMLOCK_UNSUPPORTED:                 The request is not supported.
    SIMLOCK_GENERIC_ERROR:               An error has occurred while processing
                                         the request.

===============================================================================*/
simlock_result_enum_type simlock_ck_lock(
  simlock_set_lock_ck_msg_req_type            req_data,
  simlock_callback_type                       simlock_cb_ptr,
  void                                      * user_data_ptr
);

/*=============================================================================
FUNCTION        SIMLOCK_RELOCK

DESCRIPTION
  This function allows the client to relock to the specified SIM Lock feature.
  The SIM Lock feature should have been previously locked successfully with
  valid feature data and then unlocked successfully. The same CK needs to be
  passed here as the original lock API call.

PARAMETERS
  req_data :         Data for relocking a category
  simlock_cb_ptr :   Command response call back
  user_data_ptr :    Client's user data

RETURN VALUE

  simlock_result_enum_type:

    SIMLOCK_SUCCESS:                     The command structure was properly
                                         generated and queued onto the SIM Lock
                                         Command Queue.
    SIMLOCK_INCORRECT_PARAMS:            The parameters supplied to the API are not
                                         within appropriate ranges.
    SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED: The sufficient memory is not available to
                                         process the request.
    SIMLOCK_UNSUPPORTED:                 The request is not supported.
    SIMLOCK_GENERIC_ERROR:               An error has occurred while processing
                                         the request.

===============================================================================*/
simlock_result_enum_type simlock_relock(
  simlock_relock_device_msg_req_type          req_data,
  simlock_callback_type                       simlock_cb_ptr,
  void                                      * user_data_ptr
);

/*=============================================================================
FUNCTION        SIMLOCK_UNLOCK

DESCRIPTION
  This function allows the user to unlock and disable one SIMLOCK feature on an
  identified slot.

PARAMETERS
  req_data :         Data for unlocking a category
  simlock_cb_ptr :   Command response call back
  user_data_ptr :    Client's user data

RETURN VALUE

  simlock_result_enum_type:

    SIMLOCK_SUCCESS:                     The command structure was properly
                                         generated and queued onto the SIM Lock
                                         Command Queue.
    SIMLOCK_INCORRECT_PARAMS:            The parameters supplied to the API are not
                                         within appropriate ranges.
    SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED: The sufficient memory is not available to
                                         process the request.
    SIMLOCK_UNSUPPORTED:                 The request is not supported.
    SIMLOCK_GENERIC_ERROR:               An error has occurred while processing
                                         the request.

===============================================================================*/
simlock_result_enum_type simlock_unlock(
  simlock_unlock_device_msg_req_type          req_data,
  simlock_callback_type                       simlock_cb_ptr,
  void                                      * user_data_ptr
);

/*=============================================================================
FUNCTION        SIMLOCK_GET_STATUS

DESCRIPTION
  This function retrieves all relavent SIMLOCK information and configurations
  across all slots and subscription applications including the slot and
  subscription policies.

PARAMETERS
  simlock_cb_ptr :   Command response call back
  user_data_ptr :    Client's user data

RETURN VALUE

  simlock_result_enum_type:

    SIMLOCK_SUCCESS:                     The command structure was properly
                                         generated and queued onto the SIM Lock
                                         Command Queue.
    SIMLOCK_INCORRECT_PARAMS:            The parameters supplied to the API are not
                                         within appropriate ranges.
    SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED: The sufficient memory is not available to
                                         process the request.
    SIMLOCK_UNSUPPORTED:                 The request is not supported.
    SIMLOCK_GENERIC_ERROR:               An error has occurred while processing
                                         the request.

===============================================================================*/
simlock_result_enum_type simlock_get_status(
  simlock_callback_type                       simlock_cb_ptr,
  void                                      * user_data_ptr
);

/*=============================================================================
FUNCTION        SIMLOCK_GET_CATEGORY_DATA

DESCRIPTION
  This function retrieves the SIM Lock category data for one SIM Lock feature.

PARAMETERS
  req_data :         Category and slot info for which data is requested
  simlock_cb_ptr :   Command response call back
  user_data_ptr :    Client's user data

RETURN VALUE

  simlock_result_enum_type:

    SIMLOCK_SUCCESS:                     The command structure was properly
                                         generated and queued onto the SIM Lock
                                         Command Queue.
    SIMLOCK_INCORRECT_PARAMS:            The parameters supplied to the API are not
                                         within appropriate ranges.
    SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED: The sufficient memory is not available to
                                         process the request.
    SIMLOCK_UNSUPPORTED:                 The request is not supported.
    SIMLOCK_GENERIC_ERROR:               An error has occurred while processing
                                         the request.

===============================================================================*/
simlock_result_enum_type simlock_get_category_data(
  simlock_get_category_data_msg_req_type      req_data,
  simlock_callback_type                       simlock_cb_ptr,
  void                                      * user_data_ptr
);

/*=============================================================================
FUNCTION        SIMLOCK_RUN_ALGO

DESCRIPTION
  This function runs the relevant data from EF-IMSI (3GPP), EF-IMSI_M/IMSI_T
  (3GPP2), EF-AD (3GPP), EF-GID1, and EF-GID2 from all active applications on
  the device against the SIM Lock data and configuration stored in the SIM Lock
  feature and responds with the status of which applications are allowed to
  continue to subscription ready and which applications stored invalid network
  information.

PARAMETERS
  req_data_ptr :     Data from SIM card needed to verify SIMLock
  simlock_cb_ptr :   Command response call back
  user_data_ptr :    Client's user data

RETURN VALUE

  simlock_result_enum_type:

    SIMLOCK_SUCCESS:                     The command structure was properly
                                         generated and queued onto the SIM Lock
                                         Command Queue.
    SIMLOCK_INCORRECT_PARAMS:            The parameters supplied to the API are not
                                         within appropriate ranges.
    SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED: The sufficient memory is not available to
                                         process the request.
    SIMLOCK_UNSUPPORTED:                 The request is not supported.
    SIMLOCK_GENERIC_ERROR:               An error has occurred while processing
                                         the request.

===============================================================================*/
simlock_result_enum_type simlock_run_algo(
  const simlock_run_algorithm_msg_req_type  * req_data_ptr,
  simlock_callback_type                       simlock_cb_ptr,
  void                                      * user_data_ptr
);

/*=============================================================================
FUNCTION        SIMLOCK_OTA_UNLOCK

DESCRIPTION
  This function allows to unlock 3GPP SIMLOCK features - NW, NS
  SP and CP on a device identified by the IMEI.

PARAMETERS
  req_data_ptr :     OTA data for unlocking the categories
  slot :             Physical slot
  simlock_cb_ptr :   Command response call back
  user_data_ptr :    Client's user data

RETURN VALUE

  simlock_result_enum_type:

    SIMLOCK_SUCCESS:                     The command structure was properly
                                         generated and queued onto the SIM Lock
                                         Command Queue.
    SIMLOCK_INCORRECT_PARAMS:            The parameters supplied to the API are not
                                         within appropriate ranges.
    SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED: The sufficient memory is not available to
                                         process the request.
    SIMLOCK_UNSUPPORTED:                 The request is not supported.
    SIMLOCK_GENERIC_ERROR:               An error has occurred while processing
                                         the request.

===============================================================================*/
simlock_result_enum_type simlock_ota_unlock(
  const simlock_ota_unlock_data_type            * req_data_ptr,
  simlock_slot_enum_type                          slot,
  simlock_ota_callback_type                       simlock_cb_ptr,
  void                                          * user_data_ptr
);

/*=============================================================================
FUNCTION        SIMLOCK_TEMPORARY_UNLOCK

DESCRIPTION
  This function allows to set a temporary unlock for a requested duration or
  unset an existing temporary unlock. If the function is called for a category,
  which is currently in a temporary unlocked state, then the new temporary unlock
  duration overrides the existing one.

PARAMETERS
  req_data_ptr :     Pointer to the temporary unlock request
  simlock_cb_ptr :   Command response call back
  user_data_ptr :    Client's user data

RETURN VALUE

  simlock_result_enum_type:

    SIMLOCK_SUCCESS:                     The command structure was properly
                                         generated and queued onto the SIM Lock
                                         Command Queue.
    SIMLOCK_INCORRECT_PARAMS:            The parameters supplied to the API are not
                                         within appropriate ranges.
    SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED: The sufficient memory is not available to
                                         process the request.
    SIMLOCK_UNSUPPORTED:                 The request is not supported.
    SIMLOCK_GENERIC_ERROR:               An error has occurred while processing
                                         the request.

===============================================================================*/
simlock_result_enum_type simlock_temporary_unlock(
  const simlock_temporary_unlock_msg_req_type     * req_data_ptr,
  simlock_callback_type                             simlock_cb_ptr,
  void                                            * user_data_ptr
);

/*=============================================================================
FUNCTION        SIMLOCK_SET_DEVICE_OPERATION_MODE

DESCRIPTION
  This function allows to set a device in the emergency only mode or set the
  device back in the normal operation mode.

PARAMETERS
  req_data :         device operation mode request
  simlock_cb_ptr :   Command response call back
  user_data_ptr :    Client's user data

RETURN VALUE

  simlock_result_enum_type:

    SIMLOCK_SUCCESS:                     The command structure was properly
                                         generated and queued onto the SIM Lock
                                         Command Queue.
    SIMLOCK_INCORRECT_PARAMS:            The parameters supplied to the API are not
                                         within appropriate ranges.
    SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED: The sufficient memory is not available to
                                         process the request.
    SIMLOCK_UNSUPPORTED:                 The request is not supported.
    SIMLOCK_GENERIC_ERROR:               An error has occurred while processing
                                         the request.

===============================================================================*/
simlock_result_enum_type simlock_set_device_operation_mode(
  simlock_set_device_mode_msg_req_type        req_data,
  simlock_callback_type                       simlock_cb_ptr,
  void                                      * user_data_ptr
);

/*=============================================================================
FUNCTION        SIMLOCK_ADD_LOCK_CODES_CK

DESCRIPTION
  This function allows the client to add up to 20 sets of feature data
  for a SIMLock feature that is already locked. The feature data will be
  added to the existing feature data, if the CK passed matches CK with which
  the device was locked.

PARAMETERS
  req_data :         Additional lock code data
  simlock_cb_ptr :   Command response call back
  user_data_ptr :    Client's user data

RETURN VALUE

  simlock_result_enum_type:

    SIMLOCK_SUCCESS:           The command structure was properly
                               generated and queued onto the SIM Lock
                               Command Queue.
    SIMLOCK_INCORRECT_PARAMS:  The parameters supplied to the API are not
                               within appropriate ranges.

===============================================================================*/
simlock_result_enum_type simlock_add_lock_codes_ck(
  const simlock_add_lock_codes_ck_msg_req_type    req_data,
  simlock_callback_type                           simlock_cb_ptr,
  void                                          * user_data_ptr
);

/*=============================================================================
FUNCTION        SIMLOCK_ADD_LOCK_CODES_HCK

DESCRIPTION
  This function allows the client to add up to 20 sets of feature data
  for a SIMLock feature that is already locked. The feature data will be
  added to the existing feature data, if the HCK passed matches HCK with which
  the device was locked.

PARAMETERS
  req_data :         Additional lock code data
  simlock_cb_ptr :   Command response call back
  user_data_ptr :    Client's user data

RETURN VALUE

  simlock_result_enum_type:

    SIMLOCK_SUCCESS:           The command structure was properly
                               generated and queued onto the SIM Lock
                               Command Queue.
    SIMLOCK_INCORRECT_PARAMS:  The parameters supplied to the API are not
                               within appropriate ranges.

===============================================================================*/
simlock_result_enum_type simlock_add_lock_codes_hck(
  const simlock_add_lock_codes_hck_msg_req_type    req_data,
  simlock_callback_type                            simlock_cb_ptr,
  void                                           * user_data_ptr
);

/*=============================================================================
FUNCTION        SIMLOCK_RSU_GET_ENCRYPTED_KEY

DESCRIPTION
  This function gets the modem key for the RSU. The random key is encrypted with
  a public key that is provisioned in the device.

PARAMETERS
  simlock_cb_ptr :   Command response call back
  user_data_ptr :    Client's user data

RETURN VALUE

  simlock_result_enum_type:

    SIMLOCK_SUCCESS:                     The command structure was properly
                                         generated and queued onto the SIM Lock
                                         Command Queue.
    SIMLOCK_INCORRECT_PARAMS:            The parameters supplied to the API are not
                                         within appropriate ranges.
    SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED: The sufficient memory is not available to
                                         process the request.
    SIMLOCK_UNSUPPORTED:                 The request is not supported.
    SIMLOCK_GENERIC_ERROR:               An error has occurred while processing
                                         the request.

===============================================================================*/
simlock_result_enum_type simlock_rsu_get_encrypted_key(
  simlock_rsu_callback_type           simlock_cb_ptr,
  void                              * user_data_ptr
);

/*=============================================================================
FUNCTION        SIMLOCK_RSU_SET_CONFIGURATION

DESCRIPTION
  This function sets the simlock configuration provided in the request. It
  verifies the integrity of the data in the request, before setting the
  simlock configuration.

PARAMETERS
  req_data :         Simlock configuration data
  simlock_cb_ptr :   Command response call back
  user_data_ptr :    Client's user data

RETURN VALUE

  simlock_result_enum_type:

    SIMLOCK_SUCCESS:                     The command structure was properly
                                         generated and queued onto the SIM Lock
                                         Command Queue.
    SIMLOCK_INCORRECT_PARAMS:            The parameters supplied to the API are not
                                         within appropriate ranges.
    SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED: The sufficient memory is not available to
                                         process the request.
    SIMLOCK_UNSUPPORTED:                 The request is not supported.
    SIMLOCK_GENERIC_ERROR:               An error has occurred while processing
                                         the request.

===============================================================================*/
simlock_result_enum_type simlock_rsu_set_configuration(
  simlock_data_type                       req_data,
  simlock_rsu_callback_type               simlock_cb_ptr,
  void                                  * user_data_ptr
);

/*=============================================================================
FUNCTION        SIMLOCK_VERIFY_IMSI

DESCRIPTION
  This function validates input IMSI. It goes through all IMSIs whose
  subscription SIMLOCK engine found as valid in last algo run and check whether
  input IMSI matches with any.

PARAMETERS
  imsi:         EF IMSI Data

RETURN VALUE

  simlock_result_enum_type:
    SIMLOCK_SUCCESS:        IMSI's subscription is valid.
    SIMLOCK_GENERIC_ERROR:  IMSI's subscription is not valid.
    SIMLOCK_UNSUPPORTED:    SIMLOCK feature is not defined.
===============================================================================*/
simlock_result_enum_type simlock_verify_imsi (
  simlock_imsi_type                       imsi
);

#endif /* SIMLOCK_MODEM_LIB_H */
