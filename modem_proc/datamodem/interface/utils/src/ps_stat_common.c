/*===========================================================================

                    P S _ S T A T _ C O M M O N . C

GENERAL DESCRIPTION
  This program provides the statistic access functions that can be used by
  all layers

EXTERNALIZED FUNCTIONS
  ps_stat_get_common
    This function copies the appropriate statistic to the memory passed

INITIALIZATION AND SEQUENCING REQUIREMENTS
  NONE

Copyright (c) 2005-2011 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/src/ps_stat_common.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/08/10   rp      RAM optimization changes.
04/11/07   scb    Fixed High LINT errors
10/27/05   ks      Removed compatison of stat < 0, as it is unsigned.
08/15/05   kr      Initial version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "amssassert.h"
#include "IxErrno.h"
#include "ps_stat_common.h"
#include "err.h"
#include <stringl.h>
#include "ds_Utils_DebugMsg.h"


/*===========================================================================
FUNCTION PS_STAT_GET_COMMON()

DESCRIPTION
  This function is a helper function used by all  layers to retrieve
  statistics. It returns the appropriate statistic by indexing into
  the statistic data structure through the access table passed to the
  function.

DEPENDENCIES
  None

PARAMETERS
  stat              - IN - type of stat
  access_ptr        - IN - pointer to stat access table
  stat_ptr          - IN - pointer to statistics structure
  access_table_size - IN - # of elements in the access table
  return_value      - IN - pointer for return value
                    - OUT- return value (statistics)
  ret_len           - IN - length of memory allocated for return value

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_stat_get_common
(
  uint32                             stat,
  const ps_stat_access_table_s_type *access_ptr,
  uint16                             access_table_size ,
  void                              *stat_ptr,
  void                              *return_value,
  uint16                             ret_len
)
{
  /*-------------------------------------------------------------------------
    Input parameters validation : should never fail
  -------------------------------------------------------------------------*/

  if (access_ptr == NULL)
  {
    DATA_ERR_FATAL("ps_stat_get_common(): ps_stat_get:variable access_ptr should not be NULL");
    return E_NOT_ALLOWED ;
  }

  if ( stat_ptr == NULL)
  {
    DATA_ERR_FATAL("ps_stat_get_common(): ps_stat_get:variable stat_ptr should not be NULL");
    return E_NOT_ALLOWED ;
  }

  /*-------------------------------------------------------------------------
    return value validation : should not be null
  -------------------------------------------------------------------------*/

  if (return_value == NULL)
  {
    LOG_MSG_ERROR_0("ps_stat_get_common(): "
                    "return_value should not be NULL ");
    return E_NOT_ALLOWED ;
  }

  /*-------------------------------------------------------------------------
    validate 'stat' No need to compare < 0, as stat is unsigned int.
  -------------------------------------------------------------------------*/
  if (stat > access_table_size)
  {
    LOG_MSG_ERROR_0("Invalid stat value passed to ps_stat_get_common ");
    return E_INVALID_ARG ;
  }

  /*-------------------------------------------------------------------------
    check the sizeof the stat requested. If the size is zero, the stat is
    invalid in the current context. return appropriate error
  -------------------------------------------------------------------------*/
  if (access_ptr[stat].len == 0)
  {
    LOG_MSG_ERROR_0("ps_stat_get_common(): "
                    " stat value passed in invalid in the current context");
    return E_INVALID_ARG ;
  }

  /*-------------------------------------------------------------------------
    ret_len validation - check if sufficient memory has been allocated by
    the calling program for returning statistics
  -------------------------------------------------------------------------*/

  if (ret_len < access_ptr[stat].len)
  {
    LOG_MSG_ERROR_0("ps_stat_get_common(): "
                    " Insufficient memory allocated for ptr return_value");
    return E_NOT_ALLOWED ;
  }

  /*-------------------------------------------------------------------------
    all validations complete; now copy data from statistics structure to
    memory address passed
  -------------------------------------------------------------------------*/
  memscpy(return_value,
          access_ptr[stat].len,
          (char *) stat_ptr + access_ptr[stat].offset,
          access_ptr[stat].len
         );

  return E_SUCCESS ;
}
