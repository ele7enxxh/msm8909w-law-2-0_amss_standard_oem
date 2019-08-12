/*==============================================================================

                        ds_ASTimer.h

GENERAL DESCRIPTION
  Timer extension

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/07/14    ml     Created file/Initial version.
==============================================================================*/
#include "ds_ASTimer.h"
namespace ASUtils = Appsrv::Utils;

#define DS_ASTIMER_SECONDS_MAX 4000000

ASUtils::ASTimer::ASTimer()
: seconds_remaining(0)
{
  memset(&timer, 0, sizeof(rex_timer_type));
}


ASUtils::ASTimer::~ASTimer()
{
#ifdef TEST_FRAMEWORK
  #error code not present
#endif/* TEST_FRAMEWORK */

  clear();
}


void ASUtils::ASTimer::init(rex_timer_cb_type timer_cb, uint32 user_data)
{
  rex_def_timer_ex(&timer, timer_cb, user_data);
}


void ASUtils::ASTimer::start(uint32 timeout_seconds)
{
  seconds_remaining = timeout_seconds;
  uint32 timeout = 0;

  if(seconds_remaining < DS_ASTIMER_SECONDS_MAX)
  {
    timeout = seconds_remaining;
  }
  else
  {
    timeout            = DS_ASTIMER_SECONDS_MAX;
    seconds_remaining -= DS_ASTIMER_SECONDS_MAX;
  }

  rex_set_timer(&timer, timeout * 1000);
}


bool ASUtils::ASTimer::update()
{
  if(0 == seconds_remaining)
    return true;

  start(seconds_remaining);
  return false;
}


void ASUtils::ASTimer::clear()
{
  if(0 != rex_get_timer(&timer))
  {
    rex_clr_timer(&timer);
  }
}

