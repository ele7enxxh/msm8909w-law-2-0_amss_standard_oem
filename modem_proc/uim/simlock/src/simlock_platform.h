#ifndef SIMLOCK_PLATFORM_H
#define SIMLOCK_PLATFORM_H
/*===========================================================================


            S I M L O C K   P L A T F O R M   D E F I N I T I O N S

                      A N D

                F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the simlock platform definitions.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2014, 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock_platform.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
06/17/16   vdc     Added support for SPN based SIMLOCK feature
11/05/14   am      Changes to eliminate memory leak 
09/17/14   tl      Add support for 5 and 6 parameters in message macro
07/28/14   vv      Remove unused function
04/14/14   vv      Removed un-needed macro
01/07/14   vv      Initial revision
===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "uim_msg.h"

#ifdef FEATURE_SIMLOCK

#include "mmgsdi.h"
#include "modem_mem.h"

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
#include <stringl/stringl.h>

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#else
#define SIMLOCK_MALLOC(size)  modem_mem_calloc(1, size, MODEM_MEM_CLIENT_UIM)
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#else
#define SIMLOCK_FREE(p_mem)   modem_mem_free((void*)(p_mem), MODEM_MEM_CLIENT_UIM)
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

#define SIMLOCK_MSG_HIGH_0(xx_fmt) \
   UIM_MSG_HIGH_0(xx_fmt)

#define SIMLOCK_MSG_HIGH_1(xx_fmt, xx_arg1) \
  UIM_MSG_HIGH_1(xx_fmt, xx_arg1)

#define SIMLOCK_MSG_HIGH_2(xx_fmt, xx_arg1, xx_arg2) \
  UIM_MSG_HIGH_2(xx_fmt, xx_arg1, xx_arg2)

#define SIMLOCK_MSG_HIGH_3(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  UIM_MSG_HIGH_3(xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define SIMLOCK_MSG_HIGH_4(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  UIM_MSG_HIGH_4(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

#define SIMLOCK_MSG_HIGH_5(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5) \
  UIM_MSG_LOW_5(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5)

#define SIMLOCK_MSG_HIGH_6(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6) \
  UIM_MSG_LOW_6(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6)

#define SIMLOCK_MSG_MED_0(xx_fmt) \
  UIM_MSG_MED_0(xx_fmt)

#define SIMLOCK_MSG_MED_1(xx_fmt, xx_arg1) \
  UIM_MSG_MED_1(xx_fmt, xx_arg1)

#define SIMLOCK_MSG_MED_2(xx_fmt, xx_arg1, xx_arg2) \
  UIM_MSG_MED_2(xx_fmt, xx_arg1, xx_arg2)

#define SIMLOCK_MSG_MED_3(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  UIM_MSG_MED_3(xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define SIMLOCK_MSG_MED_4(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  UIM_MSG_MED_4(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

#define SIMLOCK_MSG_MED_5(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5) \
  UIM_MSG_LOW_5(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5)

#define SIMLOCK_MSG_MED_6(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6) \
  UIM_MSG_LOW_6(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6)

#define SIMLOCK_MSG_LOW_0(xx_fmt) \
  UIM_MSG_LOW_0(xx_fmt)

#define SIMLOCK_MSG_LOW_1(xx_fmt, xx_arg1) \
  UIM_MSG_LOW_1(xx_fmt, xx_arg1)

#define SIMLOCK_MSG_LOW_2(xx_fmt, xx_arg1, xx_arg2) \
  UIM_MSG_LOW_2(xx_fmt, xx_arg1, xx_arg2)

#define SIMLOCK_MSG_LOW_3(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  UIM_MSG_LOW_3(xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define SIMLOCK_MSG_LOW_4(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  UIM_MSG_LOW_4(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

#define SIMLOCK_MSG_LOW_5(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5) \
  UIM_MSG_LOW_5(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5)

#define SIMLOCK_MSG_LOW_6(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6) \
  UIM_MSG_LOW_6(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6)

#define SIMLOCK_MSG_LOW_7(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7) \
  UIM_MSG_LOW_7(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7)

#define SIMLOCK_MSG_LOW_8(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  UIM_MSG_LOW_8(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8)

#define SIMLOCK_MSG_ERR_0(xx_fmt) \
  UIM_MSG_ERR_0(xx_fmt)

#define SIMLOCK_MSG_ERR_1(xx_fmt, xx_arg1) \
  UIM_MSG_ERR_1(xx_fmt, xx_arg1)

#define SIMLOCK_MSG_ERR_2(xx_fmt, xx_arg1, xx_arg2) \
  UIM_MSG_ERR_2(xx_fmt, xx_arg1, xx_arg2)

#define SIMLOCK_MSG_ERR_3(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  UIM_MSG_ERR_3(xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define SIMLOCK_MSG_ERR_4(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  UIM_MSG_ERR_4(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

/* ===========================================================================
   MACRO:       SIMLOCK_CHECK_AND_MEM_MALLOC
   DESCRIPTION:
   INPUT:       p_mem   : pointer to be allocated
                ptr_size: size of the allocation
  ===========================================================================*/
#define SIMLOCK_CHECK_AND_MEM_MALLOC(p_mem, ptr_size)                         \
  if(ptr_size > 0)                                                            \
  {                                                                           \
    (p_mem) = SIMLOCK_MALLOC((ptr_size));                                     \
  }

/* ===========================================================================
   MACRO:       SIMLOCK_MEM_FREE
   DESCRIPTION:
   ===========================================================================*/
#define SIMLOCK_MEM_FREE(p_mem)                                        \
  if ( (p_mem) != NULL )                                               \
  {                                                                    \
    SIMLOCK_FREE((p_mem));                                             \
    (p_mem) = NULL;                                                    \
  }

/*===========================================================================

FUNCTION SIMLOCK_MEMSCPY

DESCRIPTION
  simlock_memscpy calls memscpy to copy the contents from source buffer to
  dest buffer. memscpy compares the dest size with the number of bytes to be
  copied and returns the minimum of both.
  Error message is printed if the copied size is less than the
  number of bytes to copy.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static inline void simlock_memscpy(
  void *dst,
  size_t dst_size,
  const void *src,
  size_t copy_size
)
{
  size_t copied_size;
  copied_size = memscpy(dst, dst_size, src, copy_size);
  if(copied_size < copy_size)
  {
    SIMLOCK_MSG_ERR_2("insufficient destination buffer size. dest size: 0x%x,"
                      "src size: 0x%x", dst_size, copy_size);
  }
} /* simlock_memscpy */


/*===========================================================================

FUNCTION SIMLOCK_MEMSMOVE

DESCRIPTION
  simlock_memsmove calls memsmove to move the contents from source buffer to
  dest buffer. memsmove compares the dest size with the number of bytes to be
  moved and returns the minimum of both.
  Error message is printed if the moved size is less than the
  number of bytes to move.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static inline void simlock_memsmove(
  void *dst,
  size_t dst_size,
  const void *src,
  size_t copy_size
)
{
  size_t copied_size;
  copied_size = memsmove(dst, dst_size, src, copy_size);
  if(copied_size < copy_size)
  {
    SIMLOCK_MSG_ERR_2("insufficient destination buffer size. dest size: 0x%x,"
                      "src size: 0x%x", dst_size, copy_size);
  }
} /* simlock_memsmove */

#endif /* FEATURE_SIMLOCK */

#endif /* SIMLOCK_PLATFORM_H */
