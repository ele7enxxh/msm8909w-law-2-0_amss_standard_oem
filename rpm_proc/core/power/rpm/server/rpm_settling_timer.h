#ifndef RPM_SETTLING_TIMER_H
#define RPM_SETTLING_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdint.h>
#include "cortex-m3.h"
#include "comdef.h"
#include "rpm_config.h"

typedef void (*settling_callback)(void *ctxt);

struct settling_node
{
    uint64 settling_time;
    void   *ctxt;
    settling_callback cb;
    struct settling_node *next;
};

/* Registers a callback cb to trigger at time settling_time with parameter ctxt*/
boolean settling_timer_register(settling_callback cb, void *ctxt, uint64 settling_time);

void rpm_settling_timer_init(void);

#ifdef __cplusplus
};
#endif // __cplusplus

#endif // RPM_SETTLING_TIMER_H

