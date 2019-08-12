#ifndef PS_STAT_MEM_H
#define PS_STAT_MEM_H

/*===========================================================================

                          P S _ S T A T _ M E M . H

GENERAL DESCRIPTION
  This header file provides the structure, enum and function definitions for
  the statistics pertaining to PS_MEM.

EXTERNALIZED FUNCTIONS
  ps_stat_get_mem()
    This function supplies the required mem stats.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2005-2008 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/inc/ps_stat_mem.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/21/07   scb     Replaced the INC macro implementation to use the
                   COMMON_INC macro
12/06/05   vp      Added num_used_buffers to ps stats
08/15/05   kr      Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "comdef.h"
#include "ps_stat_common.h"
#include "ps_mem_ext.h"
#include "ps_mem.h"

#include "IxErrno.h"


/*--------------------------------------------------------------------------
  MEM - Instance stats only
---------------------------------------------------------------------------*/
typedef struct
{
  uint32 alloc_errors ;
  uint32 high_wm_exceeds ;
  uint32 max_alloc_buffers ;
  uint32 num_used_buffers;
} ps_stat_mem_i_s_type;

/*---------------------------------------------------------------------------
  MEM statistics enum
---------------------------------------------------------------------------*/
typedef enum
{
  /* Desc : # of memory allocation errors */
  /* type : uint32 */
  PS_STAT_MEM_ALLOC_ERRORS       = 0,
  /* Desc : # of times high watermark has been exceeded */
  /* type : uint32 */
  PS_STAT_MEM_HIGH_WM_EXCEEDS    = 1,
  /* Desc : max # of buffers allocated at any time */
  /* type : uint32 */
  PS_STAT_MEM_MAX_ALLOC_BUFFERS  = 2,
  /* Desc : number of buffers allocated at any time */
  /* type : uint32 */
  PS_STAT_MEM_NUM_USED_BUFFERS   = 3,
  /* Desc : all ps_mem stats */
  /* type : uint32 */
  PS_STAT_MEM_ALL
} ps_stat_mem_enum_type;


/*===========================================================================

                          LOG PACKET DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Log structure for LOG_PS_STAT_MEM_INST_C

  Common header defined in ps_stat_logging.h

  Fields are
    handle           : handle to the instance
    inst_mem_stats   : instance MEM stats
---------------------------------------------------------------------------*/
typedef struct
{
  int32                     handle;
  ps_stat_mem_i_s_type      inst_mem_stats;
} ps_stat_inst_mem_log_pkt_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_STAT_INST_DESC_MEM_TYPE

DESCRIPTION
  The structure describing a ps_mem instance.  The mem instance is contained
  within a log packet as follows:
    ps_stat_inst_desc_mem_type
    char[name_size]

  The mem instance has the mem_type struct, which contains the instance
  handle and the number of characters in the name.  The name_size field
  describes how many characters follow the struct, containing the name
  of the mem instance.

  handle    : Instance handle, index into the ps_mem pool array
  name_size : Number of characters following this containing the name of the
              memory pool
  reserved  : Used to maintain a 4-byte alignment
---------------------------------------------------------------------------*/
typedef struct
{
  uint8  handle;
  uint8  name_size;
  uint8  reserved[2];
} ps_stat_inst_desc_mem_type;


/*===========================================================================

                         EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
MACRO MEM_UPDATE_NUM_USED_BUFFERS()

DESCRIPTION
  Update the num_used_buffer stat according the current num_used for the
  pool_id.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define MEM_UPDATE_NUM_USED_BUFFERS(pool_id)                             \
{                                                                        \
  MEM_INC_INSTANCE_STATS(pool_id, num_used_buffers, 1);                  \
  if ( ps_mem_pool_info[pool_id].mem_i_stats.num_used_buffers >          \
         ps_mem_pool_info[pool_id].mem_i_stats.max_alloc_buffers )       \
  {                                                                      \
    ps_mem_pool_info[pool_id].mem_i_stats.max_alloc_buffers =            \
      ps_mem_pool_info[pool_id].mem_i_stats.num_used_buffers;            \
  }                                                                      \
}

/*===========================================================================
MACRO MEM_INC_INSTANCE_STATS()

DESCRIPTION
  This macro increments the mem statistic (field) by the necessary quantity
  (inc).

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define MEM_INC_INSTANCE_STATS(POOLID, FIELD, INC)                \
    PS_COMMON_INC_STATS(ps_mem_pool_info[(POOLID)].mem_i_stats.FIELD, 4, INC)

/*===========================================================================
MACRO MEM_DEC_INSTANCE_STATS()

DESCRIPTION
  This macro decrements the mem statistic (field) by the necessary quantity
  (inc).

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define MEM_DEC_INSTANCE_STATS(POOLID, FIELD, DEC)                \
    PS_COMMON_DEC_STATS(ps_mem_pool_info[(POOLID)].mem_i_stats.FIELD, 4, DEC)

/*===========================================================================
FUNCTION PS_STAT_GET_MEM()

DESCRIPTION
  This function supplies the required mem layer statistics. The first
  argument passed to the function is an enum that denotes the metric of
  interest.The second argument will be  a pointer to the address of the
  ps_mem_pool_info element corresponding to the pool_id. The third argument
  points to the memory location where the results will be copied.The result
  (statistic) will be copied to this memory address. The fourth argument
  passes the length of memory allocated for the return value and should be
  greater than or equal to the memory size of the statistic requested.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  stat            - IN - type of stat
  instance_ptr    - IN - Handle to instance
  return_value    - IN - pointer for return value
                  - OUT- return value (statistics)
  ret_len         - length of memory allocated

SIDE EFFECTS
  None
===========================================================================*/

errno_enum_type ps_stat_get_mem
(
  ps_stat_mem_enum_type   stat,
  void                    *instance_ptr,
  void                    *return_value,
  uint16                  ret_len
);

/*===========================================================================
FUNCTION PS_STAT_RESET_MEM()

DESCRIPTION
  This function resets the instance statistics associated with the specified
  instance.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  handle: Handle to the instance

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_reset_mem
(
  int32       handle
);

/*===========================================================================
FUNCTION PS_STAT_INIT_MEM()

DESCRIPTION

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_init_mem
(
  void
);

errno_enum_type ps_mem_get_stat_instance_all
(
  ps_stat_mem_enum_type    stat,
  void                     *pool_info_ptr,
  void                     *return_value,
  uint16                   ret_len
);

/*===========================================================================

FUNCTION PS_MEM_GET_STAT()

DESCRIPTION
  This function supplies the required mem layer statistics. The first
  argument passed to the function is an enum that denotes the metric of
  interest.The second argument will be  a pointer to the address of the
  ps_mem_pool_info element corresponding to the pool_id. The third argument
  points to the memory location where the results will be copied.The result
  (statistic) will be copied to this memory address. The fourth argument
  passes the length of memory allocated for the return value and should be
  greater than or equal to the memory size of the statistic requested.

DEPENDENCIES
  None

RETURN VALUE

  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  stat            - IN - type of stat
  instance_ptr    - IN - pointer that has the value of socket descriptor
  return_value    - IN - pointer for return value
                  - OUT- return value (statistics)
  ret_len         - length of memory allocated

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_mem_get_stat
(
  ps_stat_mem_enum_type  stat,
  void                   *instance_ptr,
  void                   *return_value,
  uint16                 ret_len
);

/*===========================================================================
FUNCTION PS_MEM_INST_FILL_DESC()

DESCRIPTION
  This function fills the description of the specified instance handle into
  a log packet.  If the handle matches the handle for all descriptions, it
  copies all of the active instance descriptions into the log packet.  If the
  handle does not specify all instances but specifies a valid active instance,
  the description associated with that instance is copied into the log packet.
  If the handle is invalid, nothing is done.

PARAMETERS
  handle          : Specifies which instance to retrieve the description from

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_mem_inst_fill_desc
(
  int32        handle
);

/*===========================================================================
FUNCTION PS_MEM_RESET_STAT()

DESCRIPTION
  This function resets the instance statistics associated with the specified
  instance.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  handle: Handle to the instance

SIDE EFFECTS
  None
===========================================================================*/
void ps_mem_reset_stat
(
  int32      handle
);

/*===========================================================================
FUNCTION PS_MEM_GET_POOLINFO_FROM_POOLID

DESCRIPTION
  This function supplies the address of the element of ps_mem_pool_info
  corresponding to poolid supplied

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  poolid       - denotes the pool number
  pool_address - a void ** pointer that is ued to pass back the address of
                 poolinfo corr. to poolid. This is a void ** so that the
                 void * value can be directlyused as the instance_ptr when
                 calling ps_stat_get_mem

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_mem_get_poolinfo_from_poolid
(
  ps_mem_pool_enum_type  poolid,
  void **                pool_address
);

#endif /* PS_STAT_MEM_H */
