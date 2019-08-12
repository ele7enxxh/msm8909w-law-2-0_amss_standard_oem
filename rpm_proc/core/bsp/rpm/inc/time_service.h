#ifndef TIME_SERVICE_H
#define TIME_SERVICE_H

#include <stdint.h>
#include "comdef.h"
#include "rpm_hwio.h"
#include "HALhwio.h"

static uint64_t time_service_now(void)
{
    uint32_t first_hi, second_hi, lo;

    INTLOCK();

    do
    {
        first_hi = HWIO_IN(RPM_F0_QTMR_V1_CNTPCT_HI);
        __schedule_barrier();

        lo = HWIO_IN(RPM_F0_QTMR_V1_CNTPCT_LO);
        __schedule_barrier();

        second_hi = HWIO_IN(RPM_F0_QTMR_V1_CNTPCT_HI);
        __schedule_barrier();
    } while(first_hi != second_hi);

    INTFREE();

    return (((uint64_t)second_hi) << 32) | lo;
}

#endif // TIME_SERVICE_H

