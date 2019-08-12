/**
  @file mre_util.c

  @brief Policy Manager utility functions.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/src/mre_util.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "mre_util.h"
#include "mre_dbg.h"
#include <atomic_ops.h>
#include "time_types.h"
#include "time_svc.h"
#include "modem_mem.h"
#include <stringl/stringl.h>

#if defined(WIN32)
#define strcasecmp  _stricmp
#endif

#if defined(TEST_FRAMEWORK)
#error code not present
#endif /* +TEST_FRAMEWORK. */



/*=============================================================================
  Memory allocation APIs
=============================================================================*/

/*-------- mre_util_mem_alloc --------*/
#if defined(TEST_FRAMEWORK)
#error code not present
#else /* +TEST_FRAMEWORK- */
void * mre_util_mem_alloc(
  size_t       size,
  const char  *pFileName,
  size_t       lineNumber
  )
#endif /* -TEST_FRAMEWORK. */
{
  void  *pMem = modem_mem_alloc_ext( size,
                                     MODEM_MEM_CLIENT_MMODE,
                                     (char *) pFileName,
                                     lineNumber );

  if (pMem == NULL)
  {
    MRE_ERR_FATAL("Unable to allocate %d bytes at line %d", size, lineNumber, 0);
  }

#if defined(TEST_FRAMEWORK)
  #error code not present
#endif /* +TEST_FRAMEWORK. */

  memset(pMem, 0, size);

  return pMem;
}


/*-------- mre_util_mem_free --------*/
void mre_util_mem_free(
  void        *pMem,
  const char  *pFileName,
  size_t       lineNumber
  )
{
  modem_mem_free_ext(pMem, MODEM_MEM_CLIENT_MMODE, (char *) pFileName, lineNumber);

#if defined(TEST_FRAMEWORK)
  #error code not present
#endif /* +TEST_FRAMEWORK. */
}



/*-------- mre_simple_dtor --------*/
void mre_simple_dtor(
  void  *pObj
)
{
  mre_mem_free(pObj);
}



/*-------- mre_str_dup --------*/
char * mre_str_dup(
  const char  *pStr
)
{
  size_t   len;
  char    *pDup = NULL;

  if (pStr != NULL)
  {
    len = strlen(pStr) + 1;
    pDup = mre_mem_alloc(len);
    memscpy(pDup, len, pStr, len);
  }

  return pDup;
}



/*=============================================================================
  Tokenization APIs.
=============================================================================*/


static boolean mre_util_is_whitespace(
  char  c
)
{
  return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}


/*-------- NEXT_CHAR --------*/
/**
@brief  Return the next character in the string.
*/
#define NEXT_CHAR             **ppStr


/*-------- END_OF_STR --------*/
/**
@brief  Evaluates to whether the current pointer to the string is at the
        the end of the string.
*/
#define END_OF_STR            (NEXT_CHAR == '\0')


/*-------- ADVANCE --------*/
/**
@brief  Advance the string pointer by one character.
*/
#define ADVANCE               ++*ppStr



/*-------- mre_util_consume_whitespace --------*/
static void mre_util_consume_whitespace(
  char const  **ppStr
)
{
  while (!END_OF_STR && mre_util_is_whitespace(NEXT_CHAR))
  {
    ADVANCE;
  }
}


/*-------- mre_get_token_ex --------*/
boolean mre_get_token_ex(
  char const  **ppStr,
  char         *pToken,
  size_t       maxSize,
  char         terminator
)
{
  size_t  index = 0;

  mre_util_consume_whitespace(ppStr);

  while (
            index < maxSize - 1
         && !END_OF_STR
         && NEXT_CHAR != terminator
         && !mre_util_is_whitespace(NEXT_CHAR)
        )
  {
    pToken[index++] = NEXT_CHAR;
    ADVANCE;
  }

  pToken[index] = '\0';

  return index > 0;
}



/*=============================================================================
  Other APIs
=============================================================================*/

/*-------- mre_util_msg_separator --------*/
void mre_util_msg_separator(
  mre_msg_separator_t which
)
{
  switch (which)
  {
    case MRE_SEPARATOR_HYPHEN:
      MRE_MSG_ERROR_0("---------------------------------------------");
      break;

    case MRE_SEPARATOR_STAR:
      MRE_MSG_ERROR_0("*********************************************");
      break;

    case MRE_SEPARATOR_UNDER:
      MRE_MSG_ERROR_0("_____________________________________________");
      break;

    case MRE_SEPARATOR_PLUS:
      MRE_MSG_ERROR_0("+++++++++++++++++++++++++++++++++++++++++++++");
      break;

    case MRE_SEPARATOR_EQUAL:
      MRE_MSG_ERROR_0("=============================================");
      break;

    default:
      break;
  }
}

/*-------- mre_util_get_time --------*/
uint32 mre_util_get_time(
  void
)
{
  time_type  time;    // Quadword

  time_get_ms(time);
  return qw_lo(time); // return the low 32-bits (ms resolution, about 1.5 days)
}
