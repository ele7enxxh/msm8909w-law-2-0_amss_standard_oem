#ifndef MMGSDI_SIMLOCK_H
#define MMGSDI_SIMLOCK_H
/*===========================================================================


                 M M G S D I   S I M L O C K  F U N C T I O N S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2014, 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.  
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header:

when       who     what, where, why
--------   ---     -----------------------------------------------------------
06/17/16   vdc     Added support for SPN based SIMLOCK feature
09/08/15   bcho    Run SIMLOCK Algo in Onchip SIM 
08/28/14   vv      Added support for unlock via REFRESH
06/30/14   vv      Remove Perso code
03/25/14   tl      Secondary revisions
02/25/14   tl      Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "uim_variation.h"
#include "comdef.h"
#include "mmgsdi.h"
#include "mmgsdilib_common.h"
#include "mmgsdi_evt.h"
#include "simlock_modem_p.h"

#ifdef FEATURE_SIMLOCK
/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/
#define MMGSDI_SIMLOCK_MAX_RETRIES        0x7FFFFFFF

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SIMLOCK_STATUE_INFO_TYPE

   DESCRIPTION:
-------------------------------------------------------------------------------*/
typedef struct {
  simlock_category_enum_type     category;
  boolean                        category_status;
} mmgsdi_simlock_status_info_type;

extern mmgsdi_simlock_status_info_type  mmgsdi_simlock_status_table
                                          [SIMLOCK_SLOT_COUNT_MAX][SIMLOCK_CATEGORY_COUNT_MAX];

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/* ==========================================================================
   FUNCTION:      MMGSDI_SIMLOCK_CONVERT_STATUS

   DESCRIPTION:
     Converts the SIM Lock status to an MMGSDI status enum type.

   DEPENDENCIES:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:
     None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_simlock_convert_status(
  simlock_result_enum_type           simlock_status
);

/* ==========================================================================
   FUNCTION:      MMGSDI_SIMLOCK_CONVERT_CATEGORY

   DESCRIPTION:
     Converts the SIM Lock category to an MMGSDI enum type.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:
     None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_simlock_convert_category(
  simlock_category_enum_type            simlock_category,
  mmgsdi_perso_feature_enum_type      * mmgsdi_category_ptr
);

/* ----------------------------------------------------------------------------
FUNCTION MMGSDI_SIMLOCK_EVT_CB

DESCRIPTION
  Callback for all MMGSDI related events that are triggered by the SIM Lock
  engine. This will be called in the SIM Lock context and the function will
  post the message back to MMGSDI.

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_simlock_evt_cb (
  const simlock_result_enum_type              status,
  const simlock_event_data_type               event_data
);

/* ----------------------------------------------------------------------------
FUNCTION MMGSDI_SIMLOCK_CB

DESCRIPTION
  Callback for all MMGSDI commands that interact with the SIM Lock engine.
  This will be called in the SIM Lock context and the function will post
  the message back to MMGSDI.

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_simlock_cb (
  simlock_result_enum_type                    status,
  const simlock_message_response_data_type  * resp_ptr,
  const void                                * user_data_ptr
);

/* ----------------------------------------------------------------------------
FUNCTION MMGSDI_SIMLOCK_PROCESS_RUN_ALGO_RESP

DESCRIPTION
  This function processes the response returned from the SIM Lock engine
  after it has run the algorithm and determines how to appropriately
  indicate the SIM Lock status for each application to the clients.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_simlock_process_run_algo_resp (
  mmgsdi_simlock_rsp_type           * simlock_rsp_ptr
);

/* ----------------------------------------------------------------------------
FUNCTION MMGSDI_SIMLOCK_PROCESS_RESPONSE

DESCRIPTION
  This function handles the response returned from the SIM Lock task.

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_simlock_process_response (
  mmgsdi_simlock_rsp_type           * simlock_rsp_ptr
);

/* ----------------------------------------------------------------------------
FUNCTION MMGSDI_SIMLOCK_PROCESS_EVENT

DESCRIPTION
  This function handles events from the SIM Lock task.

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_simlock_process_event (
  mmgsdi_simlock_evt_type           * simlock_rsp_ptr
);

/* ----------------------------------------------------------------------------
FUNCTION MMGSDI_SIMLOCK_BUILD_AND_RUN_ALGO

DESCRIPTION
  This function reads the relevant files from the MMGSDI cache, creates
  a table with the current provisioning session ids and passes the information
  to the SIM Lock engine to begin running the algorithm.
  If Onchip SIM is activated, SIMLOCK module only requires to update valid IMSI
  list so algorithm result will be ignored. 

DEPENDENCIES
  The SIM Lock task and command queue must be up and running

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_simlock_build_and_run_algo (
  boolean ignore_result
);

/* ==========================================================================
FUNCTION MGSDI_SIMLOCK_CONVERT_MMGSDI_SLOT_ID

DESCRIPTION:
  This is an utility function that converts MMGSDI slot id
  to SIMLOCK slot id

DEPENDENCIES:
  None

RETURN VALUE:
  mmgsdi_return_enum_type

SIDE EFFECTS:
  None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_simlock_convert_mmgsdi_slot_id(
  mmgsdi_slot_id_enum_type             mmgsdi_slot_id,
  simlock_slot_enum_type             * simlock_slot_id_ptr
);

/* ==========================================================================
FUNCTION MGSDI_SIMLOCK_CHECK_CATEGORY_STATUS

DESCRIPTION:
  This function checks whether a simlock category is enabled or not

DEPENDENCIES:
  None

RETURN VALUE:
  TRUE  : If the category is enabled
  FALSE : Otherwise

SIDE EFFECTS:
  None
==========================================================================*/
boolean mmgsdi_simlock_check_category_status(
  mmgsdi_slot_id_enum_type             mmgsdi_slot_id,
  simlock_category_enum_type           category
);

#endif /* FEATURE_SIMLOCK */
#endif /* MMGSDI_SIMLOCK_H */
