#ifndef MMGSDIBUILTINPLMN_H
#define MMGSDIBUILTINPLMN_H
/*===========================================================================


            M M G S D I B U I L T I N  P L M N  D E F I N I T I O N S

                      A N D   F U N C T I O N S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2014 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_builtin_plmn.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
02/25/14   bcho    Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "uim_variation.h"
#include "mmgsdi.h"


/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================
FUNCTION MMGSDI_PROCESS_SET_BUILTIN_PLMN_LIST

DESCRIPTION
  This function queues a response for handling set builtin plmn list in
  the cnf

PARAMETERS:
  mmgsdi_set_builtin_plmn_list_req_type req_ptr: request pointer

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_process_set_builtin_plmn_list(
  mmgsdi_set_builtin_plmn_list_req_type * req_ptr
);

/*================================================================
FUNCTION  MMGSDI_GET_BUILTIN_PLMN_LIST

DESCRIPTION:
  This function is called to read builtin PLMN list stored in EFS.

  INPUT PARAMETERS:
    int32 file_size_ptr    : file size pointer
    uint8 file_buffer_pptr : address of file buffer pointer

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type

SIDE EFFECTS:
  None
===============================================================*/
mmgsdi_return_enum_type mmgsdi_get_builtin_plmn_list(
  int32 * file_size_ptr,
  uint8 ** file_buffer_pptr
);

/*================================================================
FUNCTION  MMGSDI_STORE_BUILTIN_PLMN_LIST

DESCRIPTION:
  This function is called to store builtin plmn list in EFS provided by QMI NAS
  interface.

  INPUT PARAMETERS:
    mmgsdi_set_builtin_plmn_list_req_type set_builtin_plmn_list_req_ptr: request pointer

DEPENDENCIES:
  It triggers Refresh FCN on OPLMNwACT after storing builtin PLMN list only for
  SLOT1 and only when USIM/SIM app reached SUB READY.

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type

SIDE EFFECTS:
  None
===============================================================*/
mmgsdi_return_enum_type mmgsdi_store_builtin_plmn_list(
  const mmgsdi_set_builtin_plmn_list_req_type      *set_builtin_plmn_list_req_ptr
);

/*================================================================
FUNCTION  MMGSDI_BUILTIN_PLMN_LIST_IS_CONFIGURED

DESCRIPTION:
  This function is called to check biltin PLMN list is configured in EFS.

INPUT PARAMETERS:
  None

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type

SIDE EFFECTS:
  None
===============================================================*/
mmgsdi_return_enum_type mmgsdi_builtin_plmn_list_is_configured(
  void
);

#endif /* MMGSDIBUILTINPLMN_H */
