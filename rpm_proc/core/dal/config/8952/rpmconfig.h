/* Target: 8952 */

#ifndef RPM_CONFIG_H
#define RPM_CONFIG_H

// configurable RPM related constants for memory constraints
#define DAL_CONFIG_NUM_EVENTS          30  // number of events for signal and callback objects
#define DAL_CONFIG_NUM_TIMER_EVENTS    1  // number of events for timer/timeout objects
#define DAL_CONFIG_NUM_SYNC            1  // number of events for sync objects
#define DAL_CONFIG_NUM_HW_TIMERS       1

#define DAL_MEMORY_REDUCTION
#define FEATURE_WORKLOOP_NOT_USED
#define FEATURE_DAL_INTERRUPTCONTROLLER_NOT_USED

#endif /* RPM_CONFIG_H */

