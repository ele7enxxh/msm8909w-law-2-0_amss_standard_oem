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
#ifndef DS_ASTIMER_H
#define DS_ASTIMER_H

#include "rex.h"

namespace Appsrv
{
namespace Utils
{


class ASTimer
{
public:
  ASTimer();
  ~ASTimer();

  // Required
  void init(rex_timer_cb_type timer_cb, uint32 user_data);

  void start(uint32 timeout_seconds);

  // Call this after timer callback; will return true if timeout,
  // else returns false and will start another timer for the remaining.
  bool update();
  void clear();

private:
  rex_timer_type timer;
  uint32         seconds_remaining;
};


} /* namespace Utils */
} /* namespace Appsrv */

#endif /* DS_ASTIMER_H */

