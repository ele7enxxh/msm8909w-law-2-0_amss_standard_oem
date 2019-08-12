#ifndef GBA_UTIL_H
#define GBA_UTIL_H
/*===========================================================================


            G B A   U T I L   H E A D E R


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
06/10/15   tkl     Add support of IPv6 BSF address
05/12/15   tkl     Integrated review comment
05/05/15   lxu     Fixed various issues
03/29/15   tkl     Add support for TMPI
03/25/15   lxu     Use gba_url and gba_port in nv if they are valid
03/27/15   tl      Add support for GBA cancel
01/26/15   tl      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmgsdilib_common.h"

#include "gba.h"
#include "gba_lib.h"

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================
FUNCTION GBA_UTIL_GET_SESSION_INDEX

DESCRIPTION
  This is a utility function gets the session index corresponding to a
  session type and convert gba session type to mmgsdi session type

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
gba_result_enum_type gba_util_get_session_index_and_mmgsdi_session_type(
  gba_session_type                     session_type,
  uint8                              * session_index_ptr,
  mmgsdi_session_type_enum_type      * mmgsdi_session_type_ptr
);

/* ==========================================================================
FUNCTION GBA_UTIL_TRIM_SPACES

DESCRIPTION:
  This is an utility function that remove leading/trailing spaces and modify
  the input string. If the input string is allocated dynamically, caller
  should not overwrite that pointer with the returned value. Also, caller
  should not deallocate the return pointer.

DEPENDENCIES:
  None

RETURN VALUE:
  char* pointer to the string. NULL if fail to trim the string.

SIDE EFFECTS:
  trialing spaces of the input string will be trimmed, if input string has
  only space, then trimmed str = "\0".
==========================================================================*/
char* gba_util_trim_spaces
(
   char     *str_ptr
);

/* ==========================================================================
FUNCTION GBA_UTIL_ALLOC_BASE64STRING_TO_BIN

DESCRIPTION:
  Allocates memory and converts a Base64 encoded ASCII string to a binary
  atastream.

DEPENDENCIES:
  None

RETURN VALUE:
  gba_result_enum_type

SIDE EFFECTS:
  None
==========================================================================*/
uint8* gba_util_alloc_base64string_to_bin (
  const char                *input_ptr,
  uint16                    *output_len_ptr
);

/* ==========================================================================
FUNCTION GBA_UTIL_ALLOC_BIN_TO_BASE64STRING

DESCRIPTION:
  Allocates memory and converts a binary datastream to a Base64 encoded
  ASCII string format.

DEPENDENCIES:
  None

RETURN VALUE:
  char* : base64string

SIDE EFFECTS:
  None
==========================================================================*/
char* gba_util_alloc_bin_to_base64string (
  const uint8                       *input_ptr,
  uint16                             input_len
);

/*===========================================================================
FUNCTION GBA_UTIL_ITOA

DESCRIPTION
  This function converts an integer to ASCII.
  The resulting string is null terminated.

DEPENDENCIES
  The length of the returned string depends on the input_value and
  the output_size. The array pointed by 'output_string' must be
  large enough for returned string(at most equal to output_size).

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
gba_result_enum_type gba_util_itoa
(
  uint16  input_value,
  char   *output_string_ptr,
  uint8   output_size
);

#endif /* GBA_UTIL_H */
