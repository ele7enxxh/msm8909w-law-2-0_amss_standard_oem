#ifndef __POSIX_NP_H__
#define __POSIX_NP_H__

#include "pthread.h"
#include "sched.h"
#include "rex.h"

#ifdef FEATURE_QUARTZ_3X
   #define SCHED_PARAM_GET_PRIORITY(x) ((x).priority)  //REXTODO temp workaround until 3.x POSIX port fixes this
   #define pthread_attr_setthreadname pthread_attr_setname
#else
   #define SCHED_PARAM_GET_PRIORITY(x) ((x).sched_priority)
#endif

#endif //__POSIX_NP_H__
