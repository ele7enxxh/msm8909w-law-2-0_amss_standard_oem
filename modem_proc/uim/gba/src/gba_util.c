/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            G B A   U T I L   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the GBA utility functions.

                        COPYRIGHT INFORMATION

Copyright (c) 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/10/15   tkl     Add support of IPv6 BSF address
05/12/15   tkl     Integrated review comment
05/05/15   lxu     Fixed various issues
03/22/15   tkl     Add TMPI support
03/16/15   lxu     Use gba_url and gba_port in nv if they are valid
01/26/15   tl      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include <stringl/stringl.h>
#include "string.h"
#include <ctype.h>
#include "mmgsdilib_common.h"

#include "gba.h"
#include "gba_lib.h"
#include "gba_platform.h"
#include "gba_util.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define GBA_NUM_BITS_BASE64_CHAR         6
#define GBA_NUM_BITS_CHAR                8
#define GBA_STRING_MAX_LEN               256
#define GBA_MAX_ITOA_LEN                 6
#define GBA_DECIMAL_BASE                 10

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================
FUNCTION GBA_UTIL_GET_SESSION_INDEX_AND_MMGSDI_SESSION_TYPE

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
)
{
  mmgsdi_session_type_enum_type mmgsdi_session_type = MMGSDI_GW_PROV_PRI_SESSION;
  uint8                         index               = 0;

  if(mmgsdi_session_type_ptr == NULL &&
     session_index_ptr == NULL)
  {
    return GBA_GENERIC_ERROR;
  }

  switch (session_type)
  {
    case GBA_3GPP_PROV_SESSION_PRI:
      mmgsdi_session_type = MMGSDI_GW_PROV_PRI_SESSION;
      index = 0;
      break;
    case GBA_3GPP_PROV_SESSION_SEC:
      mmgsdi_session_type = MMGSDI_GW_PROV_SEC_SESSION;
      index = 1;
      break;
    case GBA_NON_PROV_SESSION_SLOT_1:
      mmgsdi_session_type = MMGSDI_NON_PROV_SESSION_SLOT_1;
      index = 2;
      break;
    case GBA_NON_PROV_SESSION_SLOT_2:
      mmgsdi_session_type = MMGSDI_NON_PROV_SESSION_SLOT_2;
      index = 3;
      break;
    default:
      GBA_MSG_LOW_1("GBA session type: 0x%x not supported", session_type);
      return GBA_GENERIC_ERROR;
  }

  if (mmgsdi_session_type_ptr != NULL)
  {
    *mmgsdi_session_type_ptr = mmgsdi_session_type;
  }

  if (session_index_ptr != NULL)
  {
    *session_index_ptr = index;
  }

  return GBA_SUCCESS;
} /* gba_util_get_session_index_and_mmgsdi_session_type */


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
  char   * str_ptr
)
{
  char *end_ptr = 0;

  if(str_ptr == NULL)
  {
    return NULL;
  }

  /* Trim leading space*/
  while(isspace(str_ptr[0]))
  {
    str_ptr++;
  }

  /* str_ptr has all spaces */
  if(str_ptr[0] == '\0')
  {
    return NULL;
  }

  /* Trim tailing spaces */
  end_ptr = str_ptr + strlen(str_ptr) - 1;

  while(end_ptr > str_ptr && isspace(end_ptr[0]))
  {
    end_ptr--;
  }
  end_ptr[1] = '\0';

  return str_ptr;
} /* gba_util_trim_spaces */


/* ==========================================================================
FUNCTION GBA_UTIL_FIND_BASE64_VALUES

DESCRIPTION:
  This is an utility function which convert a base64 string to bin

DEPENDENCIES:
  None

RETURN VALUE:
  gba_result_enum_type

SIDE EFFECTS:
  None
==========================================================================*/
static uint8 gba_util_find_base64_values
(
  boolean  find_char,
  char     input_char,
  uint8    input_index
)
{
  const char base64_table[] =
    {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"};
  const char * value_ptr = NULL;

  if (find_char)
  {
    value_ptr = strchr(base64_table, input_char);
    if (value_ptr)
    {
      return (uint8)(value_ptr - base64_table);
    }
  }
  else
  {
    if (input_index < (sizeof(base64_table)/sizeof(base64_table[0])))
    {
      return (uint8)base64_table[input_index];
    }
  }

  return 0;
} /* gba_util_find_base64_values */


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
)
{

  uint8   i           = 0;
  uint8   j           = 0;
  uint16  input_len   = 0;
  uint16  output_len  = 0;
  uint8  *output_ptr  = NULL;

  if (input_ptr == NULL || output_len_ptr == NULL)
  {
    return NULL;
  }

  /* Base64 string have to be multiples of 4 byte-blocks and
     possibly padded to a 4 bytes block */
  input_len = strnlen(input_ptr, GBA_STRING_MAX_LEN + 1);

  if ((input_len == 0) || (input_len > GBA_STRING_MAX_LEN) ||
      (input_len % 4) != 0)
  {
    GBA_MSG_ERR_1("Invalid Base64 string length: %d", input_len);
    return NULL;
  }

  output_len = (input_len / 4) * 3;

  /* Update output buffer size if input was padded with '='s
     Only 2 '=' padded bytes are allowed per 4 byte-block */
  if (input_ptr[input_len-1] == '=')
  {
    output_len--;
    if (input_ptr[input_len-2] == '=')
    {
      output_len--;
    }
  }

  output_ptr = GBA_MALLOC(sizeof(uint8) * (output_len + 1));
  if (output_ptr == NULL)
  {
    return NULL;
  }

  memset(output_ptr, 0x00, output_len + 1);

  /* Decode the string & convert to binary */
  while ((i+3) < input_len)
  {
    uint8 binary_byte = 0;

    /* Decode each block of 4 Base64 bytes to 3 binary bytes */
    uint32 first  = gba_util_find_base64_values(TRUE, input_ptr[i++], 0);
    uint32 second = gba_util_find_base64_values(TRUE, input_ptr[i++], 0);
    uint32 third  = gba_util_find_base64_values(TRUE, input_ptr[i++], 0);
    uint32 fourth = gba_util_find_base64_values(TRUE, input_ptr[i++], 0);

    uint32 all_three = (first  << (3 * GBA_NUM_BITS_BASE64_CHAR)) +
                       (second << (2 * GBA_NUM_BITS_BASE64_CHAR)) +
                       (third  << (1 * GBA_NUM_BITS_BASE64_CHAR)) +
                        fourth;

    /* 3 binary bytes */
    binary_byte = j < output_len ? ((all_three >> (2 * GBA_NUM_BITS_CHAR)) & 0xFF) : 0;
    output_ptr[j++] = binary_byte;
    binary_byte = j < output_len ? ((all_three >> (1 * GBA_NUM_BITS_CHAR)) & 0xFF) : 0;
    output_ptr[j++] = binary_byte;
    binary_byte = j < output_len ? (all_three & 0xFF) : 0;
    output_ptr[j++] = binary_byte;
  }

  *output_len_ptr = output_len;
  return output_ptr;
} /* gba_util_alloc_base64string_to_bin */


/* ==========================================================================
FUNCTION GBA_UTIL_ALLOC_BIN_TO_BASE64STRING

DESCRIPTION:
  Allocates memory and converts a binary datastream to a Base64 encoded
  ASCII string format. the output string is null terminated

DEPENDENCIES:
  None

RETURN VALUE:
  gba_result_enum_type

SIDE EFFECTS:
  None
==========================================================================*/
char* gba_util_alloc_bin_to_base64string (
  const uint8                       *input_ptr,
  uint16                             input_len
)
{
  uint8   i                = 0;
  uint8   j                = 0;
  uint8   extra_bytes      = 0;
  uint16  output_len       = 0;
  char  * output_ptr       = NULL;

  if ((input_ptr == NULL) || (input_len == 0))
  {
    GBA_MSG_ERR_1("Invalid input parameters: input_len 0x%x", input_len);
    return NULL;
  }

  /* Calculate the max buffer size needed for the encoded Base64 string,
     3 binary bytes make 4 Base64 bytes */
  output_len =  sizeof(char) + ((((input_len % 3 > 0) ? 1 : 0) +
                                  (input_len / 3 )) * 4);

  output_ptr = GBA_MALLOC(sizeof(char) * (output_len + 1));
  if (output_ptr == NULL)
  {
    return NULL;
  }

  memset(output_ptr, 0, output_len);

  /* Now encode the ASCII string to Base64 string */
  while (i < input_len)
  {
    /* Encode each block of 4 bytes from 3 ASCII bytes */
    uint32 first  = i < input_len ? input_ptr[i++] : 0;
    uint32 second = i < input_len ? input_ptr[i++] : 0;
    uint32 third  = i < input_len ? input_ptr[i++] : 0;

    uint32 all_three = (first  << (2 * GBA_NUM_BITS_CHAR)) +
                       (second << (1 * GBA_NUM_BITS_CHAR)) +
                        third;

    /* 4 Base64 bytes */
    if ((j+3) < output_len)
    {
      output_ptr[j++] = gba_util_find_base64_values(
                          FALSE, 0, (all_three >> (3 * GBA_NUM_BITS_BASE64_CHAR)) & 0x3F);
      output_ptr[j++] = gba_util_find_base64_values(
                          FALSE, 0, (all_three >> (2 * GBA_NUM_BITS_BASE64_CHAR)) & 0x3F);
      output_ptr[j++] = gba_util_find_base64_values(
                          FALSE, 0, (all_three >> (1 * GBA_NUM_BITS_BASE64_CHAR)) & 0x3F);
      output_ptr[j++] = gba_util_find_base64_values(
                          FALSE, 0, all_three & 0x3F);
    }
  }

  /* Update pading if required. It is needed if ASCII string's
     last group has either 1 or 2 bytes */
  extra_bytes = input_len % 3;
  if (extra_bytes)
  {
    uint8 bytes_to_fill = (extra_bytes == 1) ? 2 : 1;
    for (i = 1; i < 3 && bytes_to_fill; i++, --bytes_to_fill)
    {
      output_ptr[output_len - 1 - i] = '=';
    }
  }
  return output_ptr;
} /* gba_util_alloc_bin_to_base64string */


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
)
{
  char   buf[GBA_MAX_ITOA_LEN] = {'\0'};
  uint8  residue               = 0x0;
  uint8  buf_index             = sizeof(buf) - 1;

  if (output_string_ptr == NULL ||
      output_size <= 1)
  {
    return GBA_GENERIC_ERROR;
  }

  do
  {
    residue = (uint8) (input_value % GBA_DECIMAL_BASE);

    if (buf_index <= 0)
    {
      return GBA_GENERIC_ERROR;
    }

    buf[--buf_index] = (char) ( residue + '0');
  } while ( (input_value /= GBA_DECIMAL_BASE) > 0);

  while ( (*output_string_ptr++ = buf[buf_index++]) != 0)
  {
    output_size--;

    if (output_size <= 0)
    {
      return GBA_GENERIC_ERROR;
    }
  }

  return GBA_SUCCESS;
} /*  gba_util_itoa */
