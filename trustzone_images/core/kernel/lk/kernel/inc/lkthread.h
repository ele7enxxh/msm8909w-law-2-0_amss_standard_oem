/*
 * Copyright (c) 2008-2016 Travis Geiselbrecht
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef __KERNEL_THREAD_H
#define __KERNEL_THREAD_H

/*#include <sys/types.h>*/
#include "ops.h"
#include "lklist.h"
#include "lkcompiler.h"
#include "archthread.h"
#include "tzbsp.h"


/* wait queue stuff */

typedef struct wait_queue {
  magic_type magic;
  struct list_node list;
  int count;
} wait_queue_t;

/* wait queue primitive */
/* NOTE: must be inside critical section when using these */
void wait_queue_init(wait_queue_t *);

/* 
 * release all the threads on this wait queue with a return code of ERR_OBJECT_DESTROYED.
 * the caller must assure that no other threads are operating on the wait queue during or
 * after the call.
 */
void wait_queue_destroy(wait_queue_t *, bool reschedule);

/*
 * block on a wait queue.
 * return status is whatever the caller of wait_queue_wake_*() specifies.
 * a timeout other than INFINITE_TIME will set abort after the specified time
 * and return ERR_TIMED_OUT. a timeout of 0 will immediately return.
 */
status_t wait_queue_block(wait_queue_t *, time_t timeout);

/* 
 * release one or more threads from the wait queue.
 * reschedule = should the system reschedule if any is released.
 * wait_queue_error = what wait_queue_block() should return for the blocking thread.
 */
int wait_queue_wake_one(wait_queue_t *, bool reschedule, status_t wait_queue_error);
int wait_queue_wake_all(wait_queue_t *, bool reschedule, status_t wait_queue_error);

/* thread level statistics */
#define THREAD_STATS 0
#if THREAD_STATS
struct thread_stats {
  bigtime_t idle_time;
  bigtime_t last_idle_timestamp;
  int reschedules;
  int context_switches;
  int preempts;
  int yields;
  int interrupts; /* platform code increment this */
  int timer_ints; /* timer code increment this */
  int timers; /* timer code increment this */
};

extern struct thread_stats thread_stats;

#endif


/*Event specofoc definitions*/

typedef struct event {
  magic_type magic;
  bool signalled;
  uint32_t flags;
  wait_queue_t wait;
} event_t;

#define EVENT_FLAG_AUTOUNSIGNAL 1

/* Rules for Events:
 * - Events may be signaled from interrupt context *but* the reschedule
 *   parameter must be false in that case.
 * - Events may not be waited upon from interrupt context.
 * - Events without FLAG_AUTOUNSIGNAL:
 *   - Wake up any waiting threads when signaled.
 *   - Continue to do so (no threads will wait) until unsignaled.
 * - Events with FLAG_AUTOUNSIGNAL:
 *   - If one or more threads are waiting when signaled, one thread will
 *     be woken up and return.  The signaled state will not be set.
 *   - If no threads are waiting when signaled, the Event will remain
 *     in the signaled state until a thread attempts to wait (at which
 *     time it will unsignal atomicly and return immediately) or
 *     event_unsignal() is called.
*/

void event_init(event_t *, bool initial, uint32_t flags);
void event_destroy(event_t *);
status_t event_wait(event_t *);
status_t event_wait_timeout(event_t *, time_t); /* wait on the event with a timeout */
status_t event_signal(event_t *, bool reschedule);
status_t event_unsignal(event_t *);


/*Thread definitions*/

enum thread_state {
  THREAD_SUSPENDED = 0,
  THREAD_READY,
  THREAD_RUNNING,
  THREAD_BLOCKED,
  THREAD_SLEEPING,
  THREAD_DEATH,
};

typedef int (*thread_start_routine)(void *arg);

typedef void (*tzapp_int_handler_routine)(unsigned int);

typedef void (*thread_shutdown_routine)(void *pthread, unsigned int);

/* thread local storage */
enum thread_tls_list {
  MAX_TLS_ENTRY=1
};

typedef struct shared_buffer_info {
  unsigned int *address;
  unsigned int size;
} shared_buffer_type;
#define MAX_NUM_SHARED_BUFFERS 3
#define SHARED_BUFFER_ALIGNMENT 0x40 /*64 bytes*/
#define THREAD_NAME_LEN 32
#define MAX_KEY_SIZE    32
#define MAX_SEGS 5
#define QSEE_MESSAGE_CONF_KEY_LEN 16
#define QSEE_MESSAGE_INTEG_KEY_LEN 32
#define QSEE_MAX_UINT32 0xFFFFFFFF
#define QSEE_APP_HASH_LEN 32

typedef struct segments_info {
  unsigned int addr;
  unsigned int vaddr;
  unsigned int size;
  unsigned int flags;
} segments;

typedef struct tzapp_shutdown_routines tzapp_shutdown_routines;
struct tzapp_shutdown_routines {
  thread_shutdown_routine shutdown_routine;
  unsigned int shutdown_data;
  tzapp_shutdown_routines *next_routine;
};

typedef struct thread {
  magic_type magic;
  struct list_node thread_list_node;

  /* active bits */
  struct list_node queue_node;
  int priority;
  int app_id;
  int cleanup;
  int svc_thread;
  int bsp_thread;
  int blocked_on_listner_id;
  enum thread_state state;	
  int saved_critical_section_count;
  int remaining_quantum;

  /* if blocked, a pointer to the wait queue */
  struct wait_queue *blocking_wait_queue;
  status_t wait_queue_block_ret;

  event_t default_event;
  /* architecture stuff */
  struct arch_thread arch;

  /* stack stuff */
  void *stack;
  size_t stack_size;

  tzapp_int_handler_routine int_handler;

  /* entry point */
  thread_start_routine entry;
  void *arg;

  /* return code */
  uint32_t retcode;

  /* thread local storage */
  uint32_t tls[MAX_TLS_ENTRY];

  char name[THREAD_NAME_LEN];
  shared_buffer_type shared_buffers[MAX_NUM_SHARED_BUFFERS];
  segments segs[MAX_SEGS];
  uint8_t  thread_key[MAX_KEY_SIZE];
  uint8_t  tz_app_id[TZBSP_APP_ID_LEN];
  
  /* keys for qsee secure message passing*/
  uint8_t  conf_key[QSEE_MESSAGE_CONF_KEY_LEN];
  uint8_t  integ_key[QSEE_MESSAGE_INTEG_KEY_LEN];
  /* qsee application id parsed from certificate  */
  uint8_t  app_cert_id[TZBSP_APP_ID_LEN];
  /*qsee application image hash*/
  uint8_t  app_img_hash[QSEE_APP_HASH_LEN];

  /* TZ APP shutdown routines List */
  tzapp_shutdown_routines *app_shutdown_routines;

} thread_t;

typedef struct tz_app_id
{
  uint8_t  *tz_app_id;
  uint8_t   tz_app_id_len;
}thread_tz_app_id_t;

/* thread priority */
#define NUM_PRIORITIES 32
#define LOWEST_PRIORITY 0
#define HIGHEST_PRIORITY (NUM_PRIORITIES - 1)
#define DEFAULT_PRIORITY (NUM_PRIORITIES - 3)
#define IDLE_PRIORITY LOWEST_PRIORITY
#define DPC_PRIORITY (NUM_PRIORITIES / 2)

/* stack size */
#define DEFAULT_STACK_SIZE 2048
#define DPC_STACK_SIZE 1024*8


/* functions */
void thread_init_early(void);
void thread_init(void);
void thread_become_idle(void);
void thread_set_name(const char *name);
int thread_get_name(char *name, size_t size);
void thread_set_priority(int priority);
thread_t *thread_create(const char *name, thread_start_routine entry, void *arg, int priority, thread_t *t, size_t stack_size, void *stack, int app_id, tzapp_int_handler_routine int_handler, int svc_thread, int bsp_thread, uint8_t *thread_key, uint8_t *tz_app_id);
status_t thread_resume(thread_t *);
void thread_exit(int retcode) __NO_RETURN;
void thread_sleep(time_t delay);
void thread_cleanup(void *pthread);
uint8_t *thread_get_current_thread_key(void);
int thread_get_current_tz_app_id(thread_tz_app_id_t *thread_tz_app_id);

void dump_thread(thread_t *t);
void dump_all_threads(void);

/* scheduler routines */
void thread_yield(void); /* give up the cpu voluntarily */
void thread_preempt(void); /* get preempted (inserted into head of run queue) */
void thread_block(void); /* block on something and reschedule */

void thread_suspend_current(void);

/*Applications support*/
bool verify_thread_tz_app_id_avail(uint8* id);
thread_t *lookup_thread_by_id(int id);
int lookup_id_by_name(char *name);

/*Default event APIs*/
status_t event_wait_on_default_event(void);
status_t event_signal_default_event(thread_t * thread, bool reschedule);
status_t event_unsignal_default_event(void);

void thread_set_registers(thread_t * pthread, unsigned int arg0, unsigned int arg1, unsigned int arg2, unsigned int arg3, unsigned int arg4);

void *qsee_get_current_thread_reg_space(void);

/* called on every timer tick for the scheduler to do quantum expiration */
enum handler_return thread_timer_tick(void);

/* the current thread */
extern thread_t *current_thread;

/* the idle thread */
extern thread_t *idle_thread;

/* critical sections */
extern int critical_section_count;

static __inline __ALWAYS_INLINE void enter_critical_section(void)
{
  critical_section_count++;
  if (critical_section_count == 1)
    arch_disable_ints();
}

static __inline __ALWAYS_INLINE void exit_critical_section(void)
{
  critical_section_count--;
  if (critical_section_count == 0)
    arch_enable_ints();
}
 
#if 0 
static __inline __ALWAYS_INLINE bool in_critical_section(void)
{
  return critical_section_count > 0;
}

/* only used by interrupt glue */
static __inline void inc_critical_section(void) { critical_section_count++; }
static __inline void dec_critical_section(void) { critical_section_count--; }

/* thread local storage */
static __inline __ALWAYS_INLINE uint32_t tls_get(uint32_t entry)
{
  return current_thread->tls[entry];
}

static __inline __ALWAYS_INLINE uint32_t tls_set(uint32_t entry, uint32_t val)
{
  uint32_t oldval = current_thread->tls[entry];
  current_thread->tls[entry] = val;
  return oldval;
}
#endif
/* 
 * remove the thread from whatever wait queue it's in.
 * return an error if the thread is not currently blocked (or is the current thread) 
 */
status_t thread_unblock_from_wait_queue(thread_t *t, bool reschedule, status_t wait_queue_error);

#endif

