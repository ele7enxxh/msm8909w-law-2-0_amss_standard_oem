/*============================================================================

FILE:      dog.c

DESCRIPTION: RPM watchdog driver

PUBLIC CLASSES: 

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

      Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.
               All Rights Reserved.
         QUALCOMM Proprietary and Confidential

$Header: //components/rel/rpm.bf/2.1.1/core/power/rpm/dog.c#1 $
$Date: 2015/02/25 $

============================================================================*/

#include <err.h>
#include <DALDeviceId.h>
#include <DALSys.h>
#include "HALhwio.h"
#include "dog.h"
#include "dog_hwio.h"
#include "time_service.h"

void busywait (uint32 pause_time_us);

#define WATCHDOG_BARK_TIME_SCLK 0x400 // ~31.25 ms
#define WATCHDOG_BITE_TIME_SCLK 0x800 // ~62.50 ms

static uint32         dog_frozen = 0;
static dog_stats_type dog_stats;

static void dog_update_stats(dog_stats_type* dog, dog_event_type dog_event, uint32* source)
{
  dog->entries[dog->index].dog_event = dog_event;
  dog->entries[dog->index].dog_event_source = source;
  dog->entries[dog->index].timestamp = time_service_now();
  dog->index = (dog->index + 1) % DOG_STATS_ENTRIES;
}

__asm void dog_bark_isr(void) __irq
{
    IMPORT abort
    b abort
}

void dog_set_bark_time(unsigned bark_time)
{
  INTLOCK();
    while(!HWIO_INF(RPM_WDOG_BARK_TIME, SYNC_STATUS));
    HWIO_OUTF(RPM_WDOG_BARK_TIME, WDOG_BARK_VAL, bark_time);
  INTFREE();
}

void dog_set_bite_time(unsigned bite_time)
{
  INTLOCK();
    while(!HWIO_INF(RPM_WDOG_BITE_TIME, SYNC_STATUS));
    HWIO_OUTF(RPM_WDOG_BITE_TIME, WDOG_BITE_VAL, bite_time);
  INTFREE();
}

void dog_enable(unsigned enabled)
{
    enabled = enabled ? 3 : 0; // Identify appropriate settings.
    HWIO_OUT(RPM_WDOG_CTRL, enabled);
}

void dog_init(void)
{
  static boolean already_init = FALSE;

  if(already_init)
    return;

  already_init = TRUE;

  /* Start the dog ticking. */
  HWIO_OUTF(RPM_TIMERS_CLK_OFF_CTL, WDOG_TIMER_CLK_OFF, 0x0);
  busywait(153);
  dog_set_bark_time(WATCHDOG_BARK_TIME_SCLK);
  dog_set_bite_time(WATCHDOG_BITE_TIME_SCLK);
  HWIO_OUT(RPM_WDOG_CTRL, 0x3);
}

void dog_kick(void)
{
  if(!dog_frozen)
  {
    void *return_address = (void *)__return_address();	
    INTLOCK();
      while(!HWIO_INF(RPM_WDOG_RESET, SYNC_STATUS));
      HWIO_OUTF(RPM_WDOG_RESET, WDOG_RESET, 0x1);
      dog_update_stats(&dog_stats, DOG_KICK, return_address);
    INTFREE();
  }
}

