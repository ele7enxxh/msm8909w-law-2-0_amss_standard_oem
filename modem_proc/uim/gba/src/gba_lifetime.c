/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            G B A   L I F E T I M E   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the GBA lifetime handling functions.

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
07/20/15   tkl     Added HTTP Cookies support
01/26/15   tl      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "time_types.h"
#include "time_jul.h"
#include <stdlib.h>

#include "gba_lib.h"
#include "gba_platform.h"
#include "gba_time.h"

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/
/* ==========================================================================
FUNCTION GBA_LIFETIME_FIND_INDEX_OF_INPUT

DESCRIPTION:
  This function find the index of search character in the input data string.

DEPENDENCIES:
  None

RETURN VALUE:
  gba_result_enum_type

SIDE EFFECTS:
  None
==========================================================================*/
static gba_result_enum_type gba_lifetime_find_index_of_input (
  const char *                 input_data_ptr,
  uint32                       start_index,
  uint32                       end_index,
  char                         search_char,
  uint32                     * index_ptr
)
{
  uint32 i = 0;

  if(input_data_ptr == NULL ||
     index_ptr == NULL ||
     start_index > end_index)
  {
    return GBA_GENERIC_ERROR;
  }

  for(i = start_index; i <= end_index; i++)
  {
    if(input_data_ptr[i] == search_char)
    {
      *index_ptr = i;
      return GBA_SUCCESS;
    }
  }

  return GBA_GENERIC_ERROR;
} /* gba_lifetime_find_index_of_input */


/*===========================================================================
FUNCTION GBA_LIFETIME_CONVERT_TO_JULIAN

DESCRIPTION
  Converts http lifetime to julian format.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static gba_result_enum_type gba_lifetime_convert_to_julian (
  gba_lifetime_type    lifetime,
  time_julian_type   * julian_time_ptr
)
{
  char                   * temp_ptr      = NULL;
  gba_lifetime_type        temp_lifetime = lifetime;
  uint32                   index         = 0;
  uint32                   offset        = 0;
  gba_result_enum_type     gba_status    = GBA_SUCCESS;

  if(julian_time_ptr == NULL)
  {
    return GBA_GENERIC_ERROR;
  }

  /* Parse lifetime for year */
  gba_status = gba_lifetime_find_index_of_input (temp_lifetime.data,
                                             0,
                                             sizeof(temp_lifetime.data) - 1,
                                             '-',
                                             &index);
  if(gba_status != GBA_SUCCESS)
  {
    return gba_status;
  }

  temp_lifetime.data[index] = '\0';
  temp_ptr = temp_lifetime.data;
  julian_time_ptr->year = (uint16)atoi(temp_ptr);

  /* Parse lifetime for month */
  offset = index+1;
  if(offset >= GBA_MAX_LIFETIME_LEN)
  {
    return GBA_GENERIC_ERROR;
  }

  gba_status = gba_lifetime_find_index_of_input (temp_lifetime.data,
                                             offset,
                                             sizeof(temp_lifetime.data) - 1,
                                             '-',
                                             &index);
  if(gba_status != GBA_SUCCESS)
  {
    return gba_status;
  }

  temp_lifetime.data[index] = '\0';
  temp_ptr = &temp_lifetime.data[offset];
  julian_time_ptr->month = (uint16)atoi(temp_ptr);

  /* Parse lifetime for day */
  offset = index+1;
  if(offset >= GBA_MAX_LIFETIME_LEN)
  {
    return GBA_GENERIC_ERROR;
  }

  gba_status = gba_lifetime_find_index_of_input (temp_lifetime.data,
                                             offset,
                                             sizeof(temp_lifetime.data) - 1,
                                             'T',
                                             &index);
  if(gba_status != GBA_SUCCESS)
  {
    return gba_status;
  }

  temp_lifetime.data[index] = '\0';
  temp_ptr = &temp_lifetime.data[offset];
  julian_time_ptr->day = (uint16)atoi(temp_ptr);

  /* Parse lifetime for hours */
  offset = index+1;
  if(offset >= GBA_MAX_LIFETIME_LEN)
  {
    return GBA_GENERIC_ERROR;
  }

  gba_status = gba_lifetime_find_index_of_input (temp_lifetime.data,
                                             offset,
                                             sizeof(temp_lifetime.data) - 1,
                                             ':',
                                             &index);
  if(gba_status != GBA_SUCCESS)
  {
    return gba_status;
  }

  temp_lifetime.data[index] = '\0';
  temp_ptr = &temp_lifetime.data[offset];
  julian_time_ptr->hour = (uint16)atoi(temp_ptr);

  /* Parse lifetime for minutes */
  offset = index+1;
  if(offset >= GBA_MAX_LIFETIME_LEN)
  {
    return GBA_GENERIC_ERROR;
  }

  gba_status = gba_lifetime_find_index_of_input (temp_lifetime.data,
                                             offset,
                                             sizeof(temp_lifetime.data) - 1,
                                             ':',
                                             &index);
  if(gba_status != GBA_SUCCESS)
  {
    return gba_status;
  }

  temp_lifetime.data[index] = '\0';
  temp_ptr = &temp_lifetime.data[offset];
  julian_time_ptr->minute = (uint16)atoi(temp_ptr);

  /* Parse lifetime for seconds */
  offset = index+1;
  if(offset >= GBA_MAX_LIFETIME_LEN)
  {
    return GBA_GENERIC_ERROR;
  }

  gba_status = gba_lifetime_find_index_of_input (temp_lifetime.data,
                                             offset,
                                             sizeof(temp_lifetime.data) - 1,
                                             'Z',
                                             &index);
  if(gba_status != GBA_SUCCESS)
  {
    return gba_status;
  }

  temp_lifetime.data[index] = '\0';
  temp_ptr = &temp_lifetime.data[offset];
  julian_time_ptr->second = (uint16)atoi(temp_ptr);

  return GBA_SUCCESS;
} /* gba_lifetime_convert_to_julian */


/*===========================================================================
FUNCTION GBA_LIFETIME_IS_KS_EXPIRED

DESCRIPTION
  Function verifies the input lifetime has expired.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean gba_lifetime_is_ks_expired (
  gba_lifetime_type lifetime
)
{
  time_julian_type             gba_julian_time = {0};

  if(GBA_SUCCESS != gba_lifetime_convert_to_julian(lifetime, &gba_julian_time))
  {
    GBA_MSG_ERR_0("Failed to convert lifetime to julian");
    return TRUE;
  }

  return gba_time_is_time_expired(gba_julian_time, NULL);
} /* gba_lifetime_is_ks_expired */

