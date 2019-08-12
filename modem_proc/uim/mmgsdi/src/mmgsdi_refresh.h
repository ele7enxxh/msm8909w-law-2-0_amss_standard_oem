#ifndef MMGSDI_REFRESH_H
#define MMGSDI_REFRESH_H
/*===========================================================================


           M M G S D I   R E F R E S H  H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2007, 2009 - 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.  
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_refresh.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
09/07/15   av      Migrate to mcfg EFS APIs
07/28/14   am      Remove mmgsdi_refresh_remove_session() API 
07/15/14   yt      Support for NV refresh
05/08/14   hh      Correctly handle act_or_switch that results app deactivation
10/24/13   av      Fix processing of END_FAILED refresh evt for DEACT and FCN
10/18/13   yt      Clear refresh retry queue for deactivated application
01/28/13   vv      Fix compiler warnings
12/26/12   av      Do not allow writing to the file that is being refreshed
12/21/11   kk      Legacy GSDI Removal updates
02/23/11   nmb     Refresh Retry enhancements
02/22/11   ea      Fix compiler warnings
01/21/11   nmb     Added refersh retry support
08/24/10   shr     Do not send out multiple CARD_ERROR events for a slot
04/06/10   shr     Clean up REFRESH RESET and REFRESH RESET_AUTO
03/29/10   shr     Switch/deactivate provisioning clean up and redesign
03/09/10   shr     Clean up REFRESH FCN, INIT, INIT_FCN and APP_RESET
10/28/09   nb      Release 7 Enhanced Network Selection
11/12/07   sun     Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "mmgsdi.h"
#include "mmgsdilib.h"


/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/


/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/


/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_REFRESH_RETRY_TABLE_ENTRY_TYPE

   DESCRIPTION:
     This structure is used as an entry in the refresh retry table
     card_req:                    The card_req to be retried
     next:                        The next request for a table entry
-------------------------------------------------------------------------------*/
typedef struct _mmgsdi_refresh_retry_table_entry_type{
  mmgsdi_refresh_card_req_type                   card_req;
  struct _mmgsdi_refresh_retry_table_entry_type *next;
}mmgsdi_refresh_retry_table_entry_type;

/*=============================================================================

                                   MMGSDI MACROS
                        MACRO Definitions used through MMGSDI

=============================================================================*/
#define MMGSDI_REFRESH_RETRY_MODES          8
#define MMGSDI_REFRESH_RETRY_RESET          0
#define MMGSDI_REFRESH_RETRY_TABLE_SIZE    (MMGSDI_MAX_APP_INFO * MMGSDI_REFRESH_RETRY_MODES)
#define MMGSDI_REFRESH_RETRY_INACTIVE       0x00000000
#define MMGSDI_REFRESH_RETRY_ACTIVE         0x00000001
#define MMMGSI_REFRESH_RETRY_TIMER          5000
#define MMMGSI_REFRESH_RETRY_MAX_QUEUE_SIZE 10
#define MMGSDI_REFRESH_RETRY_PROV_APPS      4
#define MMGSDI_REFRESH_RETRY_MAX_RETRIES    0xFFFFFFFF


/*=============================================================================

                          FUNCTIONS

=============================================================================*/
/*===========================================================================
FUNCTION MMGSDI_REFRESH_UPDATE_CARD_STATE

DESCRIPTION
  This function updates the card state specifically during REFRESH
  RESET/RESET AUTO handling

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
void mmgsdi_refresh_update_card_state(
  mmgsdi_slot_id_enum_type slot_id);


/* ==========================================================================
  FUNCTION MMGSDI_REFRESH_SEND_EVT

  DESCRIPTION
    This function, will send events to all nodes or all clients depending on
    the refresh stage and the refresh mode

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS          - Command processed,
    MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
    MMGSDI_ERROR            - Other unknown failure

  SIDE EFFECTS
    NONE
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_refresh_send_evt(
  mmgsdi_card_refresh_extra_info_type *extra_info_ptr,
  const mmgsdi_request_header_type    *request_hdr_ptr
);


/*===========================================================================
  FUNCTION MMGSDI_REFRESH_PROCESS_EVT

  DESCRIPTION
    This function will handle the Refresh event and set the global
    mmgsdi_data_slot1/mmgsdi_data_slot2 if required.

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    Events will be sent to clients and client will in turn send
    new commands to MMGSDI that will move the overall refresh
    state
===========================================================================*/
void mmgsdi_refresh_process_evt(
  const mmgsdi_event_data_type *event_ptr);


/* =============================================================================
  FUNCTION:      MMGSDI_REFRESH_MAIN

  DESCRIPTION:
    This function handles all refresh requests  - it is also the entry into the
    refresh state machine

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
    None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_refresh_main(
  mmgsdi_refresh_req_type *req_ptr
);


/* ==========================================================================
  FUNCTION MMGSDI_INTERNAL_REFRESH_APP_RESET_AUTO_CB

  DESCRIPTION
    This function, defined as mmgsdi_callback_type, will handle
    app switch/deactivate related CNF geneted by MMGSDI

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
=============================================================================*/
void mmgsdi_internal_refresh_app_reset_auto_cb(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr);


/*===========================================================================
   FUNCTION mmgsdi_util_check_client_fcn_list

   DESCRIPTION:
     This function checks the fcn list for any additional files
     being read during a fcn refresh

   PARAMS:
    client id:    client id reading the file
    uim_file :    uim file name

   DEPENDENCIES:
     None

   LIMITATIONS:

   RETURN VALUE
    None

   SIDE EFFECTS:
     Additional File read flag is set if any files other than the ones
     listed in FCN command are read from the card
 ===========================================================================*/
void mmgsdi_util_check_client_fcn_list(
  mmgsdi_client_id_type       client_id,
  mmgsdi_file_enum_type       mmgsdi_file
);


/*===========================================================================
  FUNCTION mmgsdi_refresh_send_complete

  DESCRIPTION:
    This function will send refresh complete command to itself

  DEPENDENCIES:
     None

  LIMITATIONS:
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
 ===========================================================================*/
void mmgsdi_refresh_send_complete(
  mmgsdi_client_id_type    client_id,
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot,
  boolean                  pass_fail
);


/*===========================================================================
  FUNCTION mmgsdi_refresh_register

  DESCRIPTION:
    This function will let MMGSDI register with itself for refresh fcn

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE
   None

  SIDE EFFECTS
    None
 ===========================================================================*/
mmgsdi_return_enum_type mmgsdi_refresh_register(
  mmgsdi_client_id_type client_id,
  boolean               register_for_steering
);


/*===========================================================================
  FUNCTION MMGSDI_REFRESH_TIMER_EXPIRY

  DESCRIPTION
    This function builds a response if an external module that MMGSDI is
    dependent on does not respond

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    None

  SIDE EFFECTS
    Builds an Error Response to MMGSDI Client
===========================================================================*/
void mmgsdi_refresh_timer_expiry (
  unsigned long req_state
);

/*===========================================================================
FUNCTION MMGSDI_REFRESH_RETRY_CB

DESCRIPTION
  The defalut callback for refresh retries

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  If the retry was successful, queue the next retry req from the refresh
  retry table into the mmgsdi_cmd_q
===========================================================================*/

void mmgsdi_refresh_retry_cb(
  mmgsdi_return_enum_type mmgsdi_status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
);

/*===========================================================================
  FUNCTION MMGSDI_REFRESH_RETRY_INIT

  DESCRIPTION
    This function initializes the refresh retry settings from NV based on
    slot index.

  DEPENDENCIES
    None

  RETURN VALUE
    MMGSDI_ERROR   : If the global mmgsdi_refresh_settings_ptr has
                     already been initialized
    MMGSDI_SUCCESS : mmgsdi_refresh_settings_ptr is populated with
                     refresh retry info from NV
  SIDE EFFECTS
    None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_refresh_retry_init(
  uint8 slot_index
);

/*===========================================================================
  FUNCTION MMGSDI_REFRESH_RETRY_TIMER_EXPIRY

  DESCRIPTION
    This function is called when the refresh retry timer expires to
    queue the next retry from the refresh retry table

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    The global refresh retry index is incremented
===========================================================================*/
void mmgsdi_refresh_retry_timer_expiry(
  unsigned long slot_id
);

/*===========================================================================
FUNCTION MMGSDI_REFRESH_RETRY_PRINT_TABLE

DESCRIPTION
  Prints each populated entry in the refresh retry table

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/

void mmgsdi_refresh_retry_print_table(
  void
);

/*===========================================================================
  FUNCTION MMGSDI_REFRESH_RETRY_CLEAR_TABLE

  DESCRIPTION:
    This function will remove all requests from the Retry Table for the slot
    index

  DEPENDENCIES:
    None

  RETURN VALUE
    None

  SIDE EFFECTS:
    None
 ===========================================================================*/
void mmgsdi_refresh_retry_clear_table(
  uint8 slot_index
);

/* =============================================================================
  FUNCTION:      MMGSDI_CLEAR_REFRESH_DATA_INFO

  DESCRIPTION:
    This function will clear the global mmgsdi_refresh_data_info

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    None

  SIDE EFFECTS:
    Uses critical section
=============================================================================*/
void mmgsdi_clear_refresh_data_info(
  void
);

/* =============================================================================
  FUNCTION:      MMGSDI_COPY_INFO_TO_REFRESH_DATA_INFO

  DESCRIPTION:
    This function will copy all the data to mmgsdi_refresh_data_info if the
    refresh mode is either FCN or INIT_FCN and the number of files is more that
    zero. The data copied includes the files to be refreshed, the related aid
    and slot information.

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
    Uses critical section
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_copy_info_to_refresh_data_info(
  const mmgsdi_refresh_req_type                 *refresh_req,
  mmgsdi_slot_id_enum_type                       slot_id
);

/* ============================================================================
  FUNCTION:      MMGSDI_IS_FILE_IN_REFRESH_DATA_INFO_LIST

  DESCRIPTION:
    This function is a utility to find out if the file is present in the
    global mmgsdi_refresh_data_info that contains the files that have
    been requested to be refreshed

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    FALSE:          File is not present in the refresh files list
    TRUE:           File is present in refresh list or ERROR

  SIDE EFFECTS:
    None
=============================================================================*/
boolean mmgsdi_is_file_in_refresh_data_info_list(
  mmgsdi_session_id_type    session_id,
  mmgsdi_access_type        file_access
);

/*===========================================================================
  FUNCTION MMGSDI_REFRESH_RETRY_CLEAR_APP_ENTRIES

  DESCRIPTION:
    This function will remove all requests from the Retry Table Entry
    corresponding to the given app. Called upon deactivation of app.

  DEPENDENCIES:
    None

  RETURN VALUE
    None

  SIDE EFFECTS:
    None
 ===========================================================================*/
void mmgsdi_refresh_retry_clear_app_entries(
  mmgsdi_app_enum_type    app_type
);

/*===========================================================================
  FUNCTION MMGSDI_REFRESH_FREE_REFRESH_INFO_TYPE

  DESCRIPTION:
    This function frees memory allocated for mmgsdi_refresh_info_type
    and set the pointer to NULL

  DEPENDENCIES:
    None

  RETURN VALUE
    None

  SIDE EFFECTS:
    None
 ===========================================================================*/
void mmgsdi_refresh_free_refresh_info_type(
  mmgsdi_refresh_info_type  **refresh_info_pptr
);

/*===========================================================================
  FUNCTION MMGSDI_REFRESH_UPDATE_RETRY_SETTINGS

  DESCRIPTION
    This function re-reads the refresh retry settings from NV for the slot
    index

  DEPENDENCIES
    None

  RETURN VALUE
    MMGSDI_ERROR   : If the global mmgsdi_refresh_settings_ptr has
                     already been initialized
    MMGSDI_SUCCESS : mmgsdi_refresh_settings_ptr is populated with
                     refresh retry info from NV
  SIDE EFFECTS
    None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_refresh_update_retry_settings(
  uint8 slot_index
);
#endif /* MMGSDI_REFRESH_H */
