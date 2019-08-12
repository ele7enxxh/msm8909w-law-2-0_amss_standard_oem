#ifndef GBA_HTTP_COOKIE_H
#define GBA_HTTP_COOKIE_H
/*===========================================================================


            G B A   H T T P  C O O K I E   H E A D E R


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
06/22/15   tkl     Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "gba_lib.h"
#include "gba_http.h"
#include "gba_nv.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/


/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/
/*===========================================================================
FUNCTION GBA_HTTP_COOKIE_FREE

DESCRIPTION
  Function clears and then frees the list of cookies

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_http_cookie_free (
  void
);

/*===========================================================================
FUNCTION GBA_HTTP_COOKIE_CLEANUP

DESCRIPTION
  Function write cookies to storage and free the list of cookies

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_http_cookie_cleanup (
  void
);

/*===========================================================================
FUNCTION GBA_HTTP_COOKIE_GET_COOKIE_HEADER

DESCRIPTION
  This function checks for the IMPI assoicated with cookie, returned
  total number of cookies and copy name/value of cookies to cookie_list_ptr. caller
  should allocate memory with size of max_num_cookies * ds_http_cookie_name_s_type
  for cookie_list_ptr

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_http_cookie_get_cookie_header (
  gba_http_header_value_type            impi,
  gba_session_type                      session_type,
  uint16                                max_num_cookies,
  uint16                              * num_cookies_ptr,
  ds_http_cookie_name_s_type          * cookie_list_ptr
);

/*===========================================================================
FUNCTION GBA_HTTP_COOKIE_ADD_COOKIE

DESCRIPTION
  This function add a cookie in cookie list, ie. add
  of cookies depends on the cookie content

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
gba_result_enum_type gba_http_cookie_add_cookie (
  gba_http_header_value_type           impi,
  gba_session_type                     session_type,
  const gba_http_cookie_data_type    * cookie_ptr
);

#endif /* GBA_HTTP_COOKIE_H */

