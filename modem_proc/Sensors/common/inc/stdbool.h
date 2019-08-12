/** 
 * @file stdbool.h 
 * @brief C Standard Library boolean definitions.
 * 
 * Some toolchains (e.g. VS2008) do not contain the standard boolean definitions 
 * introduced in C99 into the C Standard Library. In these cases, the 
 * definitions are provided here instead. 
 * 
 * @see http://www.opengroup.org/onlinepubs/9699919799/basedefs/stdbool.h.html 
 * 
 * Copyright (c) 2010, 2013 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

/*==============================================================================
  Edit History

  This section contains comments describing changes made to the module. Notice
  that changes are listed in reverse chronological order. Please use ISO format
  for dates.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/common/inc/stdbool.h#1 $
  $DateTime: 2016/12/13 08:00:23 $

  when       who  what, where, why 
  ---------- ---  -----------------------------------------------------------
  2010-10-08 yk   Initial revision
==============================================================================*/

#ifndef STDBOOL_H
#define STDBOOL_H

/**
 * The Microsoft compilers up to version 1700 do not define _Bool.
 */
#if defined(_MSC_VER) && _MSC_VER <= 1700
#include <stdint.h>
typedef int_fast8_t _Bool;
#endif

/**
 * Define the Boolean macros only if they are not already defined.
 */
#ifndef __bool_true_false_are_defined
#define bool _Bool
#define false 0 
#define true 1
#define __bool_true_false_are_defined 1
#endif

#endif /*STDBOOL_H */

