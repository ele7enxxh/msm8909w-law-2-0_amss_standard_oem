#ifndef _MRE_REGEX_H_
#define _MRE_REGEX_H_

/**
  @file mre_regex.h

  @brief  Handling of regular expressions in policyman
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/inc/mre_regex.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include  "mre_set.h"

/*  A regular expression is just a set.
 */
typedef  mre_set_t mre_regex_t;


/*-------- mre_regex_create --------*/
/**
@brief  Create a regular expression obtained from parsing the string argument.

@param

@return

*/
mre_regex_t * mre_regex_create(
  char const  *pStr
);


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
);

#endif /* _MRE_REGEX_H_ */
