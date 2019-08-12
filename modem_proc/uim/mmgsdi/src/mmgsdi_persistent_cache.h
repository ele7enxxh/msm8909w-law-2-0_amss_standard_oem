#ifndef MMGSDI_PERSISTENT_CACHE_H
#define MMGSDI_PERSISTENT_CACHE_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            M M G S D I   P E R S I S T E N T  C A C H E   F U N C T I O N S


GENERAL DESCRIPTION

  This header file contains the processing of all MMGSDI persistent cache related
  functions

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2014 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_persistent_cache.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/08/14   tkl     Fix cache activate with imsi_m
09/29/14   tkl     Initial version
===========================================================================*/
/*=============================================================================

                         INCLUDE FILES FOR MODULE

=============================================================================*/
#include "mmgsdilib_common.h"
#include "mmgsdi.h"

#ifdef FEATURE_UIM_PERSISTENT_CACHE

/*=============================================================================

                       DEFINES

=============================================================================*/

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/

/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================
FUNCTION: MMGSDI_PERSISTENT_CACHE_ACTIVATE_PROFILE

DESCRIPTION
  Switches internal cache to the one specified

DEPENDENCIES
  none

RETURN VALUE
  Success/failure

SIDE EFFECTS

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_persistent_cache_activate_profile
(
  mmgsdi_slot_id_enum_type               slot_id,
  mmgsdi_iccid_info_type                 iccid,
  mmgsdi_session_type_enum_type          session_type,
  mmgsdi_data_type                       imsi_data
);

/*===========================================================================
FUNCTION: MMGSDI_PERSISTENT_CACHE_DEACTIVATE_PROFILE

DESCRIPTION
  Deactivate internal cache to per slot id and app type

DEPENDENCIES
  none

RETURN VALUE
  Success/failure

SIDE EFFECTS

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_persistent_cache_deactivate_profile
(
  mmgsdi_slot_id_enum_type               slot_id,
  mmgsdi_session_type_enum_type          session_type
);

/*===========================================================================
FUNCTION: MMGSDI_PERSISTENT_CACHE_READ


DESCRIPTION
  Reads a specific file from active profile, the function takes care of
  allocating the memory for data_ptr.

DEPENDENCIES
  none

RETURN VALUE
  Success/failure

SIDE EFFECTS

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_persistent_cache_read
(
  mmgsdi_slot_id_enum_type               slot_id,
  mmgsdi_file_enum_type                  file_enum,
  uint16                                 record_num,
  mmgsdi_len_type                       *data_len,
  uint8                                **data_ptr
);

/*===========================================================================
FUNCTION: MMGSDI_PERSISTENT_CACHE_WRITE


DESCRIPTION
  Writes to a specific file in active profile

DEPENDENCIES
  none

RETURN VALUE
  Success/failure

SIDE EFFECTS

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_persistent_cache_write
(
  mmgsdi_slot_id_enum_type               slot_id,
  mmgsdi_file_enum_type                  file_enum,
  uint16                                 record_num,
  mmgsdi_len_type                        data_len,
  const uint8                           *data_ptr
);

/*===========================================================================
FUNCTION: MMGSDI_PERSISTENT_CACHE_DELETE


DESCRIPTION
  Deletes the entire persistent cache for the slot, if active_only flag
  is set, then only the activated persistent cache for the session type
  is deleted.

DEPENDENCIES
  none

RETURN VALUE
  Success/failure

SIDE EFFECTS

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_persistent_cache_delete
(
  mmgsdi_slot_id_enum_type               slot_id,
  mmgsdi_session_type_enum_type          session_type,
  boolean                                active_only
);

#endif /* FEATURE_UIM_PERSISTENT_CACHE */
#endif /* MMGSDI_PERSISTENT_CACHE_H */
