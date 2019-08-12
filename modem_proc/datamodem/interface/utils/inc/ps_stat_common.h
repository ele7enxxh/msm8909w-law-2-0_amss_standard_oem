#ifndef PS_STAT_COMMON_H
#define PS_STAT_COMMON_H

/*===========================================================================

                    P S _ S T A T _ C O M M O N . H

GENERAL DESCRIPTION
  This header file provides the structures and functions common to all layers
  of statistics

EXTERNALIZED FUNCTIONS
  ps_stat_get_common()
    This function copies the appropriate statistic to the memory passed

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2005-2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/inc/ps_stat_common.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/08/10   rp      RAM optimization changes.
11/28/08   pp      Used more synchronized constructs for shared variable
                   manipulations.
11/28/07   pp      Removed RVCT 2.2 compiler warning[new line char at EOF].
02/17/07   scb     Added common MACROS to increment and decrement stats
                   approcpriately.
02/16/07   scb     Added new line at EOF to fix RVCT compiler warning
08/15/05   kr      Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "IxErrno.h"
#include "amssassert.h"
#include "ps_crit_sect.h"

/*---------------------------------------------------------------------------
  Definition for the common access table that will be used to store the
  access information for the statistics
---------------------------------------------------------------------------*/
typedef struct
{
  uint32 offset;
  uint32 len ;
} ps_stat_access_table_s_type ;

/*===========================================================================
MACRO FPOS_STAT()

DESCRIPTION
  This macro gives the offset of passed structutre filed 
  (inc).

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define FPOS_STAT(type,field)                                        \
 /*lint -e545 -e413 */ (((byte *)(&((type *)1)->field)) - ((byte *)((type *)1))) /*lint +e545 +e413 */

/*===========================================================================
MACRO PS_COMMON_INC_STATS()

DESCRIPTION
  This macro is used to fill Unused entries of stat tables
  It fills the offset and size as zeros in stat tables.
  Index is used only for readablility, not for usage

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define STAT_FILL_NULL(INDEX)             \
{                                         \
  0,                                      \
  0                                       \
}

/*===========================================================================
MACRO PS_COMMON_INC_STATS()

DESCRIPTION
  This macro increments the statistic (field) by the necessary quantity
  (inc).

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_COMMON_INC_STATS(FIELD, SIZE, INC)                            \
{                                                                        \
  /*lint -e{506, 774} */                                                 \
  if (sizeof(FIELD) == (SIZE))                                           \
  {                                                                      \
    (FIELD) += (INC);                                                    \
  }                                                                      \
  else                                                                   \
  {                                                                      \
    PS_ENTER_CRIT_SECTION(&global_ps_crit_section);                      \
    (FIELD) += (INC);                                                    \
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);                      \
  }                                                                      \
}

/*===========================================================================
MACRO PS_COMMON_DEC_STATS()

DESCRIPTION
  This macro decrements the statistic (field) by the necessary quantity
  (inc).

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_COMMON_DEC_STATS(FIELD, SIZE, INC)                            \
{                                                                        \
  /*lint -e{506, 774} */                                                 \
  if (sizeof(FIELD) == (SIZE))                                           \
  {                                                                      \
    (FIELD) -= (INC);                                                    \
  }                                                                      \
  else                                                                   \
  {                                                                      \
    PS_ENTER_CRIT_SECTION(&global_ps_crit_section);                      \
    (FIELD) -= (INC);                                                    \
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);                      \
  }                                                                      \
}

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
);
#endif /* PS_STAT_COMMON_H */
