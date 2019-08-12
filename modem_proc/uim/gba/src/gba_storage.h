#ifndef GBA_STORAGE_H
#define GBA_STORAGE_H
/*===========================================================================


            G B A   S T O R A G E   H E A D E R


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/20/15   tkl     Added HTTP Cookies support
04/29/15   tl      Add support for GBA_U
04/03/15   tkl     Added TMPI support
01/27/15   tl      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "gba.h"
#include "gba_lib.h"
#include "gba_http.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/

/* ----------------------------------------------------------------------------
   STRUCTURE:     GBA_STORAGE_PROFILE_DATA_TYPE

   DESCRIPTION:
     Contains the GBA profile data stored in SFS and used in the bootstrap
     procedure.
-------------------------------------------------------------------------------*/
typedef struct {
  gba_aid_type                        aid;
  gba_impi_type                       impi;
  gba_impi_type                       tmpi;
  gba_rand_type                       rand;
  gba_ks_type                         ks;
  gba_btid_type                       btid;
  gba_lifetime_type                   lifetime;
} gba_storage_profile_data_type;

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================
FUNCTION GBA_STORAGE_PUT_PROFILE

DESCRIPTION
  Function stores GBA profile information in SFS

DEPENDENCIES
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
gba_result_enum_type gba_storage_put_profile (
  gba_session_type                        session_type,
  const gba_storage_profile_data_type   * profile_data_ptr
);

/*===========================================================================
FUNCTION GBA_STORAGE_GET_PROFILE

DESCRIPTION
  Function retrieves the GBA profile information from SFS

DEPENDENCIES
  None

RETURN VALUE
  gba_storage_profile_data_type *

SIDE EFFECTS
  None
===========================================================================*/
gba_storage_profile_data_type * gba_storage_get_profile (
  gba_session_type                session_type
);

/*===========================================================================
FUNCTION GBA_STORAGE_CLEAR_PROFILE

DESCRIPTION
  Function clears the GBA profile data stored in SFS for a specific session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_storage_clear_profile (
  gba_session_type                session_type
);

/*===========================================================================
FUNCTION GBA_STORAGE_INIT

DESCRIPTION
  Function initalizes the SFS storage module.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_storage_init (
  void
);

/*===========================================================================
FUNCTION GBA_STORAGE_FREE_PROFILE

DESCRIPTION
  Function clears and then frees the storage profile data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_storage_free_profile (
  gba_storage_profile_data_type  ** profile_data_pptr
);

/*===========================================================================
FUNCTION GBA_STORAGE_PUT_COOKIES

DESCRIPTION
  Function stores GBA http cookies information in EFS.
  Storage of cookies also limit to cookies for 1 IMPI per device.

DEPENDENCIES
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
gba_result_enum_type gba_storage_put_cookies (
  const void   * data_ptr,
  uint16         data_size
);

/*===========================================================================
FUNCTION GBA_STORAGE_GET_COOKIES

DESCRIPTION
  Function retrieves the list of http cookies from EFS

DEPENDENCIES
  None

RETURN VALUE
  void *

SIDE EFFECTS
  None
===========================================================================*/
void * gba_storage_get_cookies (
  uint16      * data_size_ptr
);

/*===========================================================================
FUNCTION GBA_STORAGE_CLEAR_COOKIES

DESCRIPTION
  Function clears the cookie list data stored in EFS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_storage_clear_cookies (
  void
);

#endif /* GBA_STORAGE_H */

