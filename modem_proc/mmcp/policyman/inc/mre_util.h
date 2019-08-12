#ifndef _MRE_UTIL_H_
#define _MRE_UTIL_H_

/**
  @file mre_util.h

  @brief
*/

/*
    Copyright (c) 2013-2015 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Technologies Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this
  document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/inc/mre_util.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/


#include "comdef.h"
#include <sys/types.h>


/*=============================================================================
    Memory management functions.
=============================================================================*/

/*-------- mre_util_mem_alloc --------*/
/**
@brief  Allocate memory.  Private function - code should use use
        the mre_mem_alloc() macro in mre_util.h rather than use this directly.
*/
#if defined(TEST_FRAMEWORK)
#error code not present
#else /* +TEST_FRAMEWORK- */
void * mre_util_mem_alloc(
  size_t       size,
  const char  *pFileName,
  size_t       lineNumber
);
#endif /* -TEST_FRAMEWORK. */

/*-------- mre_util_mem_free --------*/
/**
@brief  Free memory.  Private function -  code should use the mre_mem_free()
        macro in mre_util.h rather than use this directly.
*/
void mre_util_mem_free(
  void        *pMem,
  const char  *pFileName,
  size_t       lineNumber
);


/*-------- mre_mem_alloc --------*/
/**
@brief  Allocate memory.

@param[in]  size  Size of memory buffer to allocate.

@return
  Pointer to zero-filled memory of the given size.

  This does not return if unable to allocate the requested amount of memory,
  but rather calls MRE_ERR_FATAL to print a message and exit.
*/
#if defined(TEST_FRAMEWORK)
#error code not present
#else /* +TEST_FRAMEWORK- */
#define mre_mem_alloc(size)  mre_util_mem_alloc((size), __FILENAME__, __LINE__)
#endif /* -TEST_FRAMEWORK. */


/*-------- mre_mem_free --------*/
/**
@brief  Free memory.

@param[in]  pMem        Pointer to memory allocated using mre_util_mem_alloc().

@return
  None
*/
#define mre_mem_free(pMem)   mre_util_mem_free((pMem), __FILENAME__, __LINE__)

#define MRE_MEM_FREE_IF(p)              \
          if ((p) != NULL)              \
          {                             \
            mre_mem_free((void *) (p)); \
          }                             \
          (p) = NULL




/*-------- mre_simple_dtor --------*/
/**
@brief  A destructor to be used when all that is required is to free
        the memory block that is the object.

@param[in]  pObj  Pointer to the object that is being destroyed.

@return
  Nothing.
*/
void mre_simple_dtor(
  void  *pObj
);


/*-------- mre_str_dup --------*/
/**
@brief  Duplicate a string.

Returns a pointer to memory allocated by a call to mre_mem_alloc()
that contains the string that is passed in.  Call mre_mem_free() to
free the buffer when done.

@param[in]  pStr  Pointer to string to duplicate.

@return
  Pointer to memory holding duplicated string.
*/
char * mre_str_dup(
  const char  *pStr
);




/*=============================================================================
  General purpose functions
=============================================================================*/


/*-------- mre_get_token_ex --------*/
/**
@brief

@param

@return

*/
boolean mre_get_token_ex(
  char const  **ppStr,
  char         *pToken,
  size_t        maxSize,
  char          terminator
);

#define mre_get_token(ppStr, pToken, maxSize) \
          mre_get_token_ex(ppStr, pToken, maxSize, '\0')



/*=============================================================================
  Other APIs
=============================================================================*/

typedef enum
{
  MRE_SEPARATOR_HYPHEN,         /* use '-' */
  MRE_SEPARATOR_STAR,           /* use '*' */
  MRE_SEPARATOR_UNDER,          /* use '_' */
  MRE_SEPARATOR_PLUS,           /* use '+' */
  MRE_SEPARATOR_EQUAL,          /* use '=' */
} mre_msg_separator_t;



/*-------- mre_util_msg_separator --------*/
/**
@brief  Log a separator message.

@param[in]  which   The sort of separator to use.

@return

*/
void mre_util_msg_separator(
  mre_msg_separator_t  which
);

/*-------- mre_util_get_time --------*/
/**
@brief  Returns a millisecond resolution timestamp as a 32-bit quantity.
        This has about 1.5 days of resolution -- enough for logging purposes.

@return 32-bit ms timestamp

*/
uint32 mre_util_get_time(
  void
);

#endif /* _MRE_UTIL_H_ */
