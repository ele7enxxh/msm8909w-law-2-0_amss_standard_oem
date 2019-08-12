/**
  @file mre_regex.c

  @brief  Code for regular expressions in policyman
*/

/*
    Copyright (c) 2013-2016 QUALCOMM Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/src/mre_regex.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "mre_dbg.h"
#include "mre_regex.h"
#include "mre_util.h"




/*-------- regex_elem_data_t --------*/
/**
@brief  The data type in which data for a particular regex element is stored.
        This is a union because we have to have a consistent data size to
        store in the mre_set_t that is the regex.
*/
typedef union
{
  char  literal;
  char  *pGroup;
} regex_elem_data_t;


/*-------- pfnRegexElemMatch_t --------*/
/**
@brief  Signature of match functions.
*/
typedef boolean (*regex_match_fn_t)(
  regex_elem_data_t *pData,
  char               c
);


/*-------- regex_elem_t --------*/
/**
@brief  An element of a regex consists of the function to use for matching
        and the data to be used by the match function.
*/
typedef struct
{
  regex_match_fn_t  pfnMatch;
  regex_elem_data_t data;
} regex_elem_t;



/*-----------------------------------------------------------------------------
  Functions for "any" matches.
-----------------------------------------------------------------------------*/

/*-------- regex_match_any --------*/
static boolean regex_match_any(
  regex_elem_data_t *pData,
  char               c
)
{
  /*  'any' always matches whatever is there...
   */
  return TRUE;
}


/*-------- regex_parse_any --------*/
static boolean regex_parse_any(
  regex_elem_t   *pElem,
  char const    **ppStr
)
{
  ++(*ppStr);
  pElem->pfnMatch = regex_match_any;
  return TRUE;
}


/*-----------------------------------------------------------------------------
  Functions for "group" matches.
-----------------------------------------------------------------------------*/

/*-------- regex_match_group --------*/
static boolean regex_match_group(
  regex_elem_data_t *pData,
  char               c
)
{
  return strchr(pData->pGroup, c) != NULL;
}


/*-------- regex_parse_group --------*/
static boolean regex_parse_group(
  regex_elem_t   *pElem,
  char const    **ppStr
)
{
  char const  *pStr = *ppStr;
  boolean      succeeded = FALSE;
  size_t       i;
  char         group[100];
  char         c;

  for (
        i = 0, c = *(++pStr) ;
        i < ARR_SIZE(group) - 1 && c != '\0' && c != ']' ;
        ++i, c = *(++pStr)
      )
  {
    group[i] = c;
  }

  if (i < ARR_SIZE(group) && c == ']')
  {
    succeeded = TRUE;
    group[i] = '\0';
    ++pStr;
    pElem->pfnMatch = regex_match_group;
    pElem->data.pGroup = mre_str_dup(group);
  }

  *ppStr = pStr;
  return succeeded;
}


/*-----------------------------------------------------------------------------
  Functions for "literal" matches.
-----------------------------------------------------------------------------*/

/*-------- regex_match_literal --------*/
static boolean regex_match_literal(
  regex_elem_data_t *pData,
  char               c
)
{
  return pData->literal == c;
}


/*-------- regex_parse_literal --------*/
static boolean regex_parse_literal(
  regex_elem_t   *pElem,
  char const    **ppStr
)
{
  char const  *pStr      = *ppStr;
  boolean      succeeded = FALSE;

  if (*pStr != '\0')
  {
    succeeded = TRUE;
    pElem->pfnMatch= regex_match_literal;
    pElem->data.literal = *pStr++;
  }

  *ppStr = pStr;
  return succeeded;
}


/*-----------------------------------------------------------------------------
  Functions required for the set.
-----------------------------------------------------------------------------*/

/*-------- regex_elem_compare --------*/
static boolean regex_elem_compare(
  void  *e1,
  void  *e2
)
{
  /*  Always return FALSE, so that mre_set_add will add duplicate elements.
   */
  return FALSE;
}


/*-------- regex_elem_dtor --------*/
static void regex_elem_dtor(
  void  *pe
)
{
  regex_elem_t  *pElem = (regex_elem_t *) pe;

  if (pElem->pfnMatch == regex_match_group)
  {
    mre_mem_free(pElem->data.pGroup);
  }
}



/*-----------------------------------------------------------------------------
  Parse a string into a regex.
-----------------------------------------------------------------------------*/

/*-------- mre_regex_create --------*/
mre_regex_t * mre_regex_create(
  char const  *pStr
)
{
  boolean           parseSucceeded;
  regex_elem_t      elem;
  mre_regex_t      *pRegEx;

  pRegEx = mre_set_new( sizeof(regex_elem_t),
                        12,
                        MRE_SET_NO_MAX_SIZE,
                        regex_elem_compare,
                        regex_elem_dtor );

  while (*pStr != '\0')
  {
    switch (*pStr)
    {
      case '.':
        parseSucceeded = regex_parse_any(&elem, &pStr);
        break;

      case '[':
        parseSucceeded = regex_parse_group(&elem, &pStr);
        break;

      case '\\':
        ++pStr;
        parseSucceeded = regex_parse_literal(&elem, &pStr);
        break;

      default:
        parseSucceeded = regex_parse_literal(&elem, &pStr);
        break;
    }

    if (parseSucceeded == FALSE)
    {
      ref_cnt_obj_release(pRegEx);
      pRegEx = NULL;
      break;
    }

    mre_set_add(pRegEx, &elem);
  }

  return pRegEx;
}



/*-----------------------------------------------------------------------------
  Match a string to the regex.
-----------------------------------------------------------------------------*/


/*-------- regex_do_match --------*/
static boolean regex_do_match(
  void  *e,
  void  *pCtx
)
{
  regex_elem_t   *pElem   = (regex_elem_t *) e;
  char const    **ppStr   = (char const **) pCtx;
  char            c       = **ppStr;
  boolean         isMatch = FALSE;

  /*  If we're not at the end of the string do the match.
   */
  if (c != '\0')
  {
    ++(*ppStr);
    isMatch = pElem->pfnMatch(&pElem->data, c);
  }

  return isMatch;
}


/*-------- mre_regex_match --------*/
/**
@brief  Return TRUE or FALSE depending on whether the passed string matches
        the regular expression.

@param

@return

*/
boolean mre_regex_match(
  mre_regex_t *pRegEx,
  char const  *pStr
)
{
  boolean isMatch;

  isMatch = mre_set_iterate(pRegEx, regex_do_match, &pStr);

  /*  We matched if there was a match and we're left at the end of the string.
   */
  return isMatch && *pStr == '\0';
}
