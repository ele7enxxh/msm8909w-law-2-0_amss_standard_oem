#include "lktypes.h"
typedef enum handler_return (*platform_timer_callback)(void *arg, time_t now);
status_t platform_set_periodic_timer(platform_timer_callback callback, void *arg, time_t interval);
