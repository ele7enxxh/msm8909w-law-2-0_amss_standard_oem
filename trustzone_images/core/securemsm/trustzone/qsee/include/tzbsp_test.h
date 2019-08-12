#ifndef TZBSP_TEST_H
#define TZBSP_TEST_H

/**
@file tzbsp_test.h
@brief TZBSP test definitions

@defgroup tzbsp_test TZBSP TEST definitions
@ingroup tzbsp_test
@{
*/
/*===========================================================================
  Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/include/tzbsp_test.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
09/27/10   tk       Initial version

===========================================================================*/

/*===========================================================================

          INCLUDE FILES

============================================================================*/

#include "tzbsp_log.h"

/*------------------------------------------------------------------------
                            PUBLIC TYPES
------------------------------------------------------------------------*/

#define TEST_OK(xx)                                                     \
  do                                                                    \
  {                                                                     \
    if ((xx))                                                           \
    {                                                                   \
      PRINT("%s:%d Failed!\n", __func__, __LINE__);                     \
      tzbsp_abort(__FILE__ ":%s:%d:" #xx, __func__, __LINE__);          \
    }                                                                   \
    else                                                                \
      PRINT("%s:%d Passed\n", __func__, __LINE__);                      \
  } while(0)

#define TEST_FAIL(xx)                                                   \
  do                                                                    \
  {                                                                     \
    if (!(xx))                                                          \
    {                                                                   \
      PRINT("%s:%d Failed!", __func__, __LINE__);                       \
      tzbsp_abort(__FILE__ ":%s:%d:" #xx, __func__, __LINE__);          \
    }                                                                   \
    else                                                                \
    {                                                                   \
      PRINT("%s:%d Passed", __func__, __LINE__);                        \
    }                                                                   \
  } while(0)

/* Test for false boolean return values. */
#define TEST_FALSE(xx)                                                  \
  do                                                                    \
  {                                                                     \
    if ((xx))                                                           \
    {                                                                   \
      PRINT("%s:%d Failed!\n", __func__, __LINE__);                     \
      tzbsp_abort(__FILE__ ":%s:%d:" #xx, __func__, __LINE__);          \
    }                                                                   \
    else                                                                \
      PRINT("%s:%d Passed\n", __func__, __LINE__);                      \
  } while(0)

/* Test for true boolean return values. */
#define TEST_TRUE(xx)                                                   \
  do                                                                    \
  {                                                                     \
    if (!(xx))                                                          \
    {                                                                   \
      PRINT("%s:%d Failed!", __func__, __LINE__);                       \
      tzbsp_abort(__FILE__ ":%s:%d:" #xx, __func__, __LINE__);          \
    }                                                                   \
    else                                                                \
    {                                                                   \
      PRINT("%s:%d Passed", __func__, __LINE__);                        \
    }                                                                   \
  } while(0)

#define PRINT(fmt, ...) tzbsp_log(TZBSP_MSG_DEBUG, "CPU %d: " fmt, \
                                  tzbsp_cur_cpu_num(),  ##__VA_ARGS__)

/** @} */

#endif /* TZBSP_TEST_H */
