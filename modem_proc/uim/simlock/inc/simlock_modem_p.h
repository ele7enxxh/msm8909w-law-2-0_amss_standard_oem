#ifndef SIMLOCK_MODEM_P_H
#define SIMLOCK_MODEM_P_H
/*===========================================================================


            S I M L O C K   D E F I N I T I O N S

                      A N D   F U N C T I O N S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2014 - 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     -----------------------------------------------------------
06/04/15   bcho    Run algorithm after a lock operation
10/21/14   vv      Add support for late simlock config registration
10/10/14   vv      Add support for remote simlock configuration
09/29/14   vv      Added support for the remote SFS
09/29/14   vv      Added support for emergency only mode
09/17/14   tl      Add support for perso and deperso secure
08/29/14   vv      Added support for temporary unlock
06/04/14   vv      Remove FEATURE_SIMLOCK
03/25/14   tl      Secondary revisions
02/25/14   tl      Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "uim_variation.h"
#include "comdef.h"
#include "simlock_common.h"
#include "simlock_modem_lib.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/
/* ----------------------------------------------------------------------------
   DATA TYPE:      simlock_token_id_type
------------------------------------------------------------------------------*/
typedef uint16 simlock_token_id_type;

/* ----------------------------------------------------------------------------
   ENUM:      SIMLOCK_EVENT_ENUM_TYPE

   DESCRIPTION: The events from the SIMLock
-------------------------------------------------------------------------------*/
typedef enum {
  SIMLOCK_UNLOCK_EVENT,
  SIMLOCK_TEMPORARY_UNLOCK_EVENT,
  SIMLOCK_DEVICE_MODE_EVENT,
  SIMLOCK_REGISTER_REMOTE_SFS_EVENT,
  SIMLOCK_LOCK_EVENT
} simlock_event_enum_type;

/* ----------------------------------------------------------------------------
   ENUM:      SIMLOCK_REGISTRATION_ENUM_TYPE

   DESCRIPTION: This enum specifies registration request type
-------------------------------------------------------------------------------*/
typedef enum {
  SIMLOCK_REGISTER,
  SIMLOCK_DEREGISTER
} simlock_registration_enum_type;

/* ----------------------------------------------------------------------------
   ENUM:      SIMLOCK_REGISTRATION_REQUEST_ENUM_TYPE

   DESCRIPTION: This enum specifies if the registration is for events or
                remote operations
-------------------------------------------------------------------------------*/
typedef enum {
  SIMLOCK_REGISTRATION_EVENT,
  SIMLOCK_REGISTRATION_REMOTE_SFS
} simlock_registration_request_enum_type;

/* ----------------------------------------------------------------------------
   ENUM:      SIMLOCK_FILE_ENUM_TYPE

   DESCRIPTION: This enum specifies the simlock file types
-----------------------------------------------------------------------------*/
typedef enum {
  SIMLOCK_FILE_TYPE_CONFIG,
  SIMLOCK_FILE_TYPE_CATEGORY
} simlock_file_enum_type;

/* ----------------------------------------------------------------------------
   ENUM:      SIMLOCK_FILE_OPERATION_ENUM_TYPE

   DESCRIPTION:
     This enum specifies the type of operation that needs to be performed
     on the simlock data
-------------------------------------------------------------------------------*/
typedef enum {
  SIMLOCK_FILE_OPERATION_READ,
  SIMLOCK_FILE_OPERATION_WRITE
} simlock_file_operation_enum_type;

/* ----------------------------------------------------------------------------
   ENUM:      SIMLOCK_REMOTE_SFS_REQUEST_ENUM_TYPE

   DESCRIPTION:
     This enum specifies the remote SFS request type
-----------------------------------------------------------------------------*/
typedef enum {
  SIMLOCK_REMOTE_SFS_DISABLE,
  SIMLOCK_REMOTE_SFS_ENABLE
} simlock_remote_sfs_request_enum_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      SIMLOCK_UNLOCK_EVENT_TYPE

   DESCRIPTION:
     This structure contains the unlock information from the unlock event
-------------------------------------------------------------------------------*/
typedef struct {
  uint32                             num_retries_max;
  uint32                             curr_retries;
  simlock_slot_enum_type             slot;
  simlock_category_enum_type         category;
} simlock_unlock_event_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      SIMLOCK_GENERIC_EVENT_TYPE

   DESCRIPTION:
     This structure contains the slot and category information from
     an event (e.g. temporary unlock event or lock event)
-------------------------------------------------------------------------------*/
typedef struct {
  simlock_slot_enum_type             slot;
  simlock_category_enum_type         category;
} simlock_generic_event_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      SIMLOCK_DEVICE_MODE_EVENT_TYPE

   DESCRIPTION:
     This structure contains the device mode information from
     the device mode event
-------------------------------------------------------------------------------*/
typedef struct {
  simlock_device_mode_enum_type     mode;
} simlock_device_mode_event_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      SIMLOCK_EVENT_DATA_TYPE

   DESCRIPTION:
     This structure is for the various event messages from SIM Lock
-------------------------------------------------------------------------------*/
typedef struct {
  /* The triggered event */
  simlock_event_enum_type                   evt_type;
  union{
    simlock_unlock_event_type               unlock;
    simlock_generic_event_type              temporary_unlock;
    simlock_device_mode_event_type          device_mode;
    simlock_generic_event_type              lock;
  } message;
} simlock_event_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      SIMLOCK_FILE_TYPE

   DESCRIPTION:
     This structure contains simlock data file type
-------------------------------------------------------------------------------*/
typedef struct
{
  simlock_file_enum_type          file_type;
  union
  {
    struct
    {
      simlock_slot_enum_type      slot;
      simlock_category_enum_type  category;
    } category_data;
  } data;
} simlock_file_type;

/* ----------------------------------------------------------------------------
   DATA TYPE:      SIMLOCK_EVT_CALLBACK_TYPE
-------------------------------------------------------------------------------*/
/** Event callback function registered by a client to receive SIM Lock events.
*/
typedef void (*simlock_evt_cb_type) (const simlock_result_enum_type status,
                                     const simlock_event_data_type  event_data);

/* ----------------------------------------------------------------------------
   DATA TYPE:      SIMLOCK_SET_REMOTE_SFS_CALLBACK_TYPE
-----------------------------------------------------------------------------*/
/** @brief Client callback function.

    The parameter to the callback function contains the results of the
    procedures performed at the client's request.
*/
typedef void (*simlock_set_remote_sfs_callback_type) (simlock_result_enum_type      status,
                                                      const void                  * user_data_ptr);

/* ----------------------------------------------------------------------------
   DATA TYPE:      SIMLOCK_REMOTE_SFS_RESPONSE_CALLBACK_TYPE
-------------------------------------------------------------------------------*/
/** response callback function passed in to perform remote storage
    (read/write) operations on simlock configuration data */
typedef void (*simlock_remote_sfs_response_cb_type) (simlock_result_enum_type            status,
                                                     simlock_token_id_type               token,
                                                     const simlock_data_type           * simlock_data_ptr);

/* ----------------------------------------------------------------------------
   DATA TYPE:      SIMLOCK_REMOTE_SFS_REQUEST_CALLBACK_TYPE
-------------------------------------------------------------------------------*/
/** callback function registered by a client to perform remote storage
    (read/write) operations on simlock configuration data */
typedef simlock_result_enum_type (*simlock_remote_sfs_request_cb_type) (simlock_file_operation_enum_type       operation,
                                                                        simlock_file_type                      file,
                                                                        simlock_token_id_type                  token,
                                                                        const simlock_data_type              * simlock_data_ptr,
                                                                        simlock_remote_sfs_response_cb_type    simlock_remote_resp_cb);

/*=============================================================================

                                  FUNCTIONS

=============================================================================*/

/*=============================================================================
FUNCTION        SIMLOCK_REGISTER_EVENT

DESCRIPTION
  This internal API will register the client callback with the SIM Lock task
  so that certain commands will trigger indications to the client.

PARAMETERS
  operation       : Register or deregister from SIM Lock
  simlock_evt_cb  : Callback to be registered and involked

RETURN VALUE
  simlock_result_enum_type:

    SIMLOCK_SUCCESS:           The command structure was properly
                               generated and queued onto the SIM Lock
                               Command Queue.
    SIMLOCK_INCORRECT_PARAMS:  The parameters supplied to the API are not
                               within appropriate ranges.

===============================================================================*/
simlock_result_enum_type simlock_register_event(
  simlock_registration_enum_type           operation,
  simlock_evt_cb_type                      simlock_evt_cb
);

/*=============================================================================
FUNCTION        SIMLOCK_REGISTER_REMOTE_OPERATION

DESCRIPTION
  This internal API will register the client callback with the SIM Lock task
  so that remote SFS storage operations (read/write) on the simlock data
  can be performed.

PARAMETERS 
  registration  : Register or deregister from SIM Lock
  client_cb     : Callback to be registered and involked

RETURN VALUE
  simlock_result_enum_type:

    SIMLOCK_SUCCESS:           The command structure was properly
                               generated and queued onto the SIM Lock
                               Command Queue.
    SIMLOCK_INCORRECT_PARAMS:  The parameters supplied to the API are not
                               within appropriate ranges.

===============================================================================*/
simlock_result_enum_type simlock_register_remote_sfs_operation(
  simlock_registration_enum_type                registration,
  simlock_remote_sfs_request_cb_type            client_cb
);

/*=============================================================================
FUNCTION        SIMLOCK_UNLOCK_TRUSTED

DESCRIPTION
  This function allows the user to unlock and disable one SIMLOCK feature on an
  identified slot. The difference between this function and simlock_unlock() is
  that it will be exposed to trusted sources and allow the trusted caller to unlock
  a feature without a CK value.

PARAMETERS
  req_data       :   Data for unlocking a category
  simlock_cb_ptr :   Command response call back
  user_data_ptr  :   Client's user data

RETURN VALUE
  simlock_result_enum_type:

    SIMLOCK_SUCCESS:           The command structure was properly
                               generated and queued onto the SIM Lock
                               Command Queue.
    SIMLOCK_INCORRECT_PARAMS:  The parameters supplied to the API are not
                               within appropriate ranges.

===============================================================================*/
simlock_result_enum_type simlock_unlock_trusted(
  simlock_unlock_device_msg_req_type          req_data,
  simlock_callback_type                       simlock_cb_ptr,
  void                                      * user_data_ptr
);

/*=============================================================================
FUNCTION        SIMLOCK_SET_REMOTE_SFS

DESCRIPTION
  This function allows to move the simlock data from a local SFS to remote
  SFS or vice-versa.

PARAMETERS
  req_data       : Enable or disable remote SFS
  simlock_cb_ptr : Command response call back
  user_data_ptr  : Client's user data

RETURN VALUE
  simlock_result_enum_type:

    SIMLOCK_SUCCESS:           The command structure was properly
                               generated and queued onto the SIM Lock
                               Command Queue.
    SIMLOCK_INCORRECT_PARAMS:  The parameters supplied to the API are not
                               within appropriate ranges.

===============================================================================*/
simlock_result_enum_type simlock_set_remote_sfs (
  simlock_remote_sfs_request_enum_type         req_data,
  simlock_set_remote_sfs_callback_type         simlock_cb_ptr,
  void                                       * user_data_ptr
);

#endif /* SIMLOCK_MODEM_P_H */
