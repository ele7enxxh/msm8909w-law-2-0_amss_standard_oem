
#ifndef UIMDIAG_UTIL_H
#define UIMDIAG_UTIL_H
/*===========================================================================

===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2009, 2011-2014 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdiag/inc/uimdiag_util.h#1 $$ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who   what, where, why
--------   ---   -----------------------------------------------------------
05/22/16   vdc   Remove F3 messages for memory allocation failure
01/10/14   df    Use v2.0 diag macros
04/24/13   rr    Featurized optional logs
02/18/12   av    Cleaned up remaining ALWAYS OFF features
08/05/11   av    Added UIMDIAGUTIL_TMC_MEM_FREE_NULL_OK
12/22/10   yt    Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
12/23/09   nb    Updated UIMDIAGUTIL_DEBUG_MSG_ERR for TESTFRAMEWORK
10/18/09   adp   Adding UIMDIAGUTIL_DEBUG_MSG_ERR , UIMDIAG_TOUINT64 and
                 MEM_FREE Macros
09/02/09   adp   Initial Revision -Added initial mmgsdi session based commands


=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "uim_variation.h"
#include "uimdiag.h"
#include "uim_msg.h"


/*=============================================================================

                       DEFINES DECLARATIONS

=============================================================================*/


/* ===========================================================================-
   DESCRIPTION: Macros for Optional logging. FEATURE_UIMDIAG_OPTIONAL_LOGS
                needs to be turned on for the messages to be logged.
   ===========================================================================*/
#ifdef FEATURE_UIMDIAG_OPTIONAL_LOGS

#define UIMDIAG_MSG_LOW_0(str)       UIM_MSG_LOW_0(str)
#define UIMDIAG_MSG_MED_0(str)       UIM_MSG_MED_0(str)
#define UIMDIAG_MSG_HIGH_0(str)      UIM_MSG_HIGH_0(str)
#define UIMDIAG_MSG_ERROR_0(str)     UIM_MSG_ERR_0(str)
#define UIMDIAG_MSG_LOW_1(str,a)     UIM_MSG_LOW_1(str,a)
#define UIMDIAG_MSG_MED_1(str,a)     UIM_MSG_MED_1(str,a)
#define UIMDIAG_MSG_HIGH_1(str,a)    UIM_MSG_HIGH_1(str,a)
#define UIMDIAG_MSG_ERROR_1(str,a)   UIM_MSG_ERR_1(str,a)
#define UIMDIAG_MSG_LOW_2(str,a,b)   UIM_MSG_LOW_2(str,a,b)
#define UIMDIAG_MSG_MED_2(str,a,b)   UIM_MSG_MED_2(str,a,b)
#define UIMDIAG_MSG_HIGH_2(str,a,b)  UIM_MSG_HIGH_2(str,a,b)
#define UIMDIAG_MSG_ERROR_2(str,a,b) UIM_MSG_ERR_2(str,a,b)

#else

#define UIMDIAG_MSG_LOW_0(str)
#define UIMDIAG_MSG_MED_0(str)
#define UIMDIAG_MSG_HIGH_0(str)
#define UIMDIAG_MSG_ERROR_0(str)
#define UIMDIAG_MSG_LOW_1(str,a)
#define UIMDIAG_MSG_MED_1(str,a)
#define UIMDIAG_MSG_HIGH_1(str,a)
#define UIMDIAG_MSG_ERROR_1(str,a)
#define UIMDIAG_MSG_LOW_2(str,a,b)
#define UIMDIAG_MSG_MED_2(str,a,b)
#define UIMDIAG_MSG_HIGH_2(str,a,b)
#define UIMDIAG_MSG_ERROR_2(str,a,b)

#endif /* FEATURE_UIMDIAG_OPTIONAL_LOGS */

/* ===========================================================================*/

#define UIMDIAG_LODWORD(l) ((uint32)((uint64)(l)))
#define UIMDIAG_HIDWORD(l) ((uint32)(((uint64)(l)>>32)&0xFFFFFFFF))
#define UIMDIAG_TOUINT64(result,low32,high32)               \
        result = (uint64)((uint64) (low32)) << 32;          \
        result = (uint64)(result) | (high32);

/* ===========================================================================-
   MACRO:       UIMDIAGUTIL_DEBUG_MSG_ERR
   DESCRIPTION: When UIMDIAGUTIL_DEBUG_MSG_ERR is define, an ERROR_FATAL
                is called so that the Errors can be caught and further
                analyzed.
                These Errors are logged to NV for Further Analysis.
   ===========================================================================*/
#define UIMDIAGUTIL_DEBUG_MSG_ERR_0(str_ptr)        \
  UIM_MSG_ERR_0("UIMDIAG: (" str_ptr ")")
#define UIMDIAGUTIL_DEBUG_MSG_ERR_2(str_ptr,a,b)        \
  UIM_MSG_ERR_2("UIMDIAG: (" str_ptr ")",(a),(b))
#define UIMDIAGUTIL_DEBUG_MSG_ERR_3(str_ptr,a,b,c)      \
  UIM_MSG_ERR_3("UIMDIAG: (" str_ptr ")",(a),(b),(c))

/* ===========================================================================
   MACRO:       UIMDIAG_UTIL_MSG_MALLOC_DEBUG_LOG
   DESCRIPTION: Will print a message when the debug feaure is enabled
                else it is just an empty stub.
   ===========================================================================*/

#ifdef FEATURE_UIM_MODEM_HELP_DEBUG
  #define UIMDIAG_UTIL_MSG_MALLOC_DEBUG_LOG(p_mem , ptr_size)     \
    UIM_MSG_HIGH_2("MODEM Malloc 0x%x, size=0x%x", (p_mem), (ptr_size));
#else
  #define UIMDIAG_UTIL_MSG_MALLOC_DEBUG_LOG(p_mem , ptr_size)
#endif/* FEATURE_UIM_MODEM_HELP_DEBUG */

/* ===========================================================================
   MACRO:       UIMDIAG_UTIL_RETURN_IF_NULL
   DESCRIPTION: Will check the parameter provided and return
                UIMDIAG_INCORRECT_PARAM if the pointer is NULL
   ===========================================================================*/
#define UIMDIAG_UTIL_RETURN_IF_NULL(p_var)                      \
  if ( (p_var) == NULL  )                                       \
  {                                                             \
    UIM_MSG_ERR_0("Attempt to use NULL POINTER");               \
    return UIMDIAG_INCORRECT_PARAM;                             \
  }

/* ===========================================================================
   MACRO:       UIMDIAG_UTIL_RETURN_IF_NULL_2
   DESCRIPTION: Will check the parameters provided and return
                UIMDIAG_INCORRECT_PARAM if any of the pointer is NULL
   ===========================================================================*/
/*lint -emacro(774,UIMDIAG_UTIL_RETURN_IF_NULL_2) */
#define UIMDIAG_UTIL_RETURN_IF_NULL_2(p_var1, p_var2)                    \
  {                                                                        \
    uint8 is_null = 0;                                                     \
    if ((p_var1) == NULL)                                                  \
    {                                                                      \
      is_null = 1;                                                         \
    }                                                                      \
    else if ((p_var2) == NULL)                                             \
    {                                                                      \
      is_null = 2;                                                         \
    }                                                                      \
    if (is_null > 0)                                                       \
    {                                                                      \
      UIM_MSG_ERR_1("Attempt to use NULL POINTER : 0x%x", is_null);        \
      return UIMDIAG_INCORRECT_PARAM;                                      \
    }                                                                      \
  }

/* ===========================================================================
   MACRO:       UIMDIAG_UTIL_RETURN_IF_NULL_3
   DESCRIPTION: Will check the parameters provided and return
                UIMDIAG_INCORRECT_PARAM if any of the pointer is NULL
   ===========================================================================*/
/*lint -emacro(774,UIMDIAG_UTIL_RETURN_IF_NULL_3) */
#define UIMDIAG_UTIL_RETURN_IF_NULL_3(p_var1, p_var2, p_var3)          \
  {                                                                    \
    uint8 is_null = 0;                                                 \
    if ((p_var1) == NULL)                                              \
    {                                                                  \
      is_null = 1;                                                     \
    }                                                                  \
    else if ((p_var2) == NULL)                                         \
    {                                                                  \
      is_null = 2;                                                     \
    }                                                                  \
    else if ((p_var3) == NULL)                                         \
    {                                                                  \
      is_null = 3;                                                     \
    }                                                                  \
    if (is_null > 0)                                                   \
    {                                                                  \
      UIM_MSG_ERR_1("Attempt to use NULL POINTER : 0x%x", is_null);    \
      return UIMDIAG_INCORRECT_PARAM;                                  \
    }                                                                  \
  }

/* ===========================================================================
   MACRO:       UIMDIAG_ASSERT
   DESCRIPTION: Will check first paramter provided is FALSE and return
                "return_param" (usually UIMDIAG_ERROR) if the first
                parameter is FALSE
   ===========================================================================*/
#define UIMDIAG_ASSERT(p_var, return_param)                     \
  if ( (p_var) == FALSE  )                                      \
  {                                                             \
    UIM_MSG_ERR_0("Assertion Failed");                          \
    return return_param;                                        \
  }

/* ===========================================================================
   MACRO:       UIMDIAG_UTIL_TMC_MEM_MALLOC_AND_VALIDATE
   DESCRIPTION: Allocate TMC memory according to the input size
                Check if memory allocation is successful or not
                memset the memory location to 0x00
   INPUT:       p_mem: pointer to be allocated
                size: size of the allocation
  ===========================================================================*/
#define UIMDIAG_UTIL_TMC_MEM_MALLOC_AND_VALIDATE(p_mem, ptr_size)           \
  if ((signed int)ptr_size > 0 )                                            \
  {                                                                         \
    (p_mem) = uimdiag_malloc((ptr_size));                                   \
    UIMDIAG_UTIL_MSG_MALLOC_DEBUG_LOG(p_mem , ptr_size)                     \
  }

/* ===========================================================================
   MACRO:       UIMDIAG_TMC_MEM_FREE
   DESCRIPTION: Will Free the TMC Memory and peform the necessary checks for
                NULL.  It will also assign the Pointer to NULL.
   ===========================================================================*/
/*lint -emacro(774,UIMDIAGUTIL_TMC_MEM_FREE) */
#define UIMDIAGUTIL_TMC_MEM_FREE(p_mem)                          \
  if ( (p_mem) != NULL )                                         \
  {                                                              \
    UIMDIAGUTIL_MSG_FREE_DEBUG_LOG(p_mem);                       \
    uimdiag_free((void*)(p_mem));                                \
    (p_mem) = NULL;                                              \
  }

#ifdef FEATURE_UIM_MODEM_HELP_DEBUG
  #define UIMDIAGUTIL_MSG_MALLOC_DEBUG_LOG(p_mem , ptr_size)     \
    UIM_MSG_HIGH_2("MODEM uimdiag Malloc 0x%x, size=0x%x", (p_mem), (ptr_size));
#else
  #define UIMDIAGUTIL_MSG_MALLOC_DEBUG_LOG(p_mem , ptr_size)
#endif/* FEATURE_UIM_MODEM_HELP_DEBUG */

#ifdef FEATURE_UIM_MODEM_HELP_DEBUG
  #define UIMDIAGUTIL_MSG_FREE_DEBUG_LOG(p_mem )                 \
    UIM_MSG_HIGH_1("uimdiag Mem Free 0x%x", (p_mem));
#else
  #define UIMDIAGUTIL_MSG_FREE_DEBUG_LOG(p_mem )
#endif/* FEATURE_UIM_MODEM_HELP_DEBUG */


/* ===========================================================================
   MACRO:       UIMDIAG_UTIL_RETURN_IF_OUT_OF_RANGE
   DESCRIPTION: Will return UIMDIAG_OUT_OF_BOUNDS if var does not fall
                within [min, max]
   ===========================================================================*/
#define UIMDIAG_UTIL_RETURN_IF_OUT_OF_RANGE(Var,Min,Max)                        \
   if ( ((Var) < (Min)) || ((Var) > (Max)) )                                   \
  {                                                                             \
    UIMDIAGUTIL_DEBUG_MSG_ERR_3("Value 0x%x out of range MIN: 0x%x, MAX: 0x%x", \
                               (Var),(Min),(Max));                              \
    return UIMDIAG_OUT_OF_BOUNDS;                                               \
  }
#endif /* UIMDIAG_UTIL_H */
