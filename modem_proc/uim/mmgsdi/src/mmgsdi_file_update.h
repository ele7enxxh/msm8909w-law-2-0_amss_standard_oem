#ifndef MMGSDI_FILE_UPDATE_H
#define MMGSDI_FILE_UPDATE_H
/*===========================================================================


           M M G S D I   F I L E   U P D A T E   H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2012 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.  
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. 

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_file_update.h#1 $$ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/23/12   vv      Initial revision
 
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
 
 
/*=============================================================================

                          FUNCTIONS

=============================================================================*/
 
/* =============================================================================
  FUNCTION:      MMGSDI_PROCESS_FILE_UPDATE_REGISTRATION

  DESCRIPTION:
    This function handles file update reg/de-reg requests

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
mmgsdi_return_enum_type mmgsdi_process_file_update_registration(
  mmgsdi_session_register_file_update_req_type *req_ptr
);

/* ============================================================================
  FUNCTION:      MMGSDI_PROCESS_FILE_UPDATE_REGISTRATION_RSP

  DESCRIPTION:
    This function will process file update reg/dereg response
     
  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The function was successful is sending the response
    MMGSDI_ERROR:            Could not get client index/
                             Could not build confirmation/
                             Other error
  
  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_process_file_update_registration_rsp(
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
);

/* ============================================================================
  FUNCTION:      MMGSDI_BUILD_FILE_UPDATE_EVENT

  DESCRIPTION:
    This function will build the file update notification whenever there is
    an update to a file on the card.
     
  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    None
  
  SIDE EFFECTS:
    None
=============================================================================*/
void mmgsdi_build_file_update_event(
  mmgsdi_return_enum_type              status,
  const mmgsdi_write_extra_info_type * write_extra_ptr,
  mmgsdi_session_id_type               session_id,
  mmgsdi_slot_id_enum_type             slot
);
#endif /* MMGSDI_FILE_UPDATE_H */
