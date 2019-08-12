#include <stringl/stringl.h>
#include "qurt_cycles.h"
#include "qurt_timer.h"
#include <npa.h>
#include "rex.h"
#include "rex_port.h"
#include "rex_os_port.h"
#include "rex_restricted.h"
#include "log.h"
#include "rexlog.h"

/*=================================================================================================
                                       DATA DECLARATIONS
=================================================================================================*/
#define Q6_DEFAULT_FREQ 595200 /* in KHz */
#define Q6SW_NPA_RESOURCE_NAME "/clk/cpu"

#ifndef BLAST_MAX_HTHREAD_LIMIT
   #define BLAST_MAX_HTHREAD_LIMIT 6
#endif

#define LOG_BLAST_TASKPROFILE_C_VER         3
#define LOG_BLAST_TASKPROFILE_C_APP         0
#define LOG_SYSPROFILE_C_VER                1
#define LOG_BLAST_TASKPROFILE_C_REX_TASK    0
#define LOG_BLAST_TASKPROFILE_C_IDLE_TASK   1

/* Absolute time that our profiling window starts. Moved forward in rex_os_profiling_clear_data. */
static uint32 rextp_tick_base = 0;
static uint64 rextp_pcycle_base = 0;
volatile uint32 core_clk_freq = 0;
static npa_event_handle npa_evt_handle = NULL;

/* this is a global variable provided by BLAST */
extern unsigned int QURT_MAX_HTHREADS;
uint64 idle_bases[BLAST_MAX_HTHREAD_LIMIT];
/*=================================================================================================
                                       FORWARD DECLARATIONS
=================================================================================================*/
extern void dog_kick(void);

void rextp_core_clk_freq_cb
( 
   void           *context,
   unsigned int   event_type, 
   void           *data,       
   unsigned int   data_size
)
{
  npa_event_data *event_data;
  
   // check event_type
   if(event_type == NPA_EVENT_CHANGE)
   {
      event_data = (npa_event_data *)data;

      if (NULL != event_data)
      {
         //check data->resource_name      
         if (strcmp(event_data->resource_name, Q6SW_NPA_RESOURCE_NAME) == 0)
         {
            //check data->state, which is freq in KHz
            core_clk_freq = event_data->state;
         }
      }
   }
}

uint32 rextp_core_clk_freq_query(void)
{
   npa_query_status status;
   npa_query_type query_result;

   // Query the resource for the current state.
   status = npa_query_by_name(Q6SW_NPA_RESOURCE_NAME, NPA_QUERY_CURRENT_STATE, &query_result);
   if (NPA_QUERY_SUCCESS == status)
   {
      // The value query_result.data.state has the result of the query.
      return query_result.data.state;
   }
   else
   {
      return 0;
   }
}

void rextp_npa_event_register(void)
{
   /**
      @note create the event - 
      Register a callback that will be invoked everytime the cpu clock frequency changes.  
      Return handle will be NULL if event could not be created
   */
   npa_evt_handle =  npa_create_change_event_cb(
                           Q6SW_NPA_RESOURCE_NAME, 
                           "CPU Freq change event",   /* Event Name - info only */
                           rextp_core_clk_freq_cb,    /* Callback Function */
                           NULL);

   /* read current core clock frequency */
   core_clk_freq = rextp_core_clk_freq_query();
   if (0 == core_clk_freq)
   {
      core_clk_freq = Q6_DEFAULT_FREQ; 
   }
}

void rextp_npa_event_deregister(void)
{
   /* destroy the event when done */
   if (NULL != npa_evt_handle)
   {
      npa_destroy_event_handle(npa_evt_handle);
   }
}

void rex_os_init_profiling(void)
{
   /* Do nothing */
}

static void rex_os_clear_tp(rex_os_tcb_type *p_os_tcb, unsigned int ignored)
{
   /* Init the task's profiling base (or start) time */
   qurt_profile_reset_threadid_pcycles(rex_os_get_rtos_thread_id(p_os_tcb));
}

static void rex_os_profiling_clear_data(void)
{
   /**
   @note System ticks should always be the logest. So first 
   collect ticks then pcycles followed by resetting thread 
   run pcycles.
   */
   rextp_tick_base = qurt_sysclock_get_hw_ticks();
   rextp_pcycle_base = qurt_get_core_pcycles();

   /* Iterate through the list of tasks and initialize their base (or start)
   ** timestamps before collecting any profiling information.
   */
   rex_task_list_mgr_for_each(rex_os_clear_tp, 0);
   
   /* Reset OS P cycle counters */
   qurt_profile_reset_idle_pcycles();
}

void rex_os_start_task_profiling(void)
{
   rex_os_profiling_clear_data();
   rextp_npa_event_register();
   /* Enable collection of profiling data for every thread */
   qurt_profile_enable(1);
}

void rex_os_stop_task_profiling(void)
{
   rextp_npa_event_deregister();
   /* Disable collection of profiling data for every thread */
   qurt_profile_enable(0);
}

static void rex_os_collect_tp(rex_os_tcb_type *p_os_tcb, unsigned int ignored)
{
   LOG_BLAST_TASKPROFILE_C_type *tp_ptr;
   uint64 pcycles[BLAST_MAX_HTHREAD_LIMIT];
   uint64 this_pcycle_count;
   uint32 this_timestamp;

   /* Allocate a log record */
   tp_ptr = (LOG_BLAST_TASKPROFILE_C_type *)log_alloc(LOG_BLAST_TASKPROFILE_C,
                                       sizeof(LOG_BLAST_TASKPROFILE_C_type));

   /* Fill in the log if allocation worked */
   if( tp_ptr != NULL )
   {
      /* version no. */
      tp_ptr->ver = LOG_BLAST_TASKPROFILE_C_VER;
      /* App - 0 - Modem SW, 1 - Modem FW, 2 - Multi Media */
      tp_ptr->app = LOG_BLAST_TASKPROFILE_C_APP;
      /* Special task is "0" for normal tasks */
      tp_ptr->special_task = LOG_BLAST_TASKPROFILE_C_REX_TASK;
      /* Flags */
      tp_ptr->flags        |= LOG_TASKPROFILE_REX_SLICE_TYPE_BIT;

      //REXTODO implement suspended
      // if(tcb_ptr->suspended)
      // {
         // tp_ptr->flags |= LOG_TASKPROFILE_REX_SUSPENDED_BIT;
      // }

      /* Task ID */
      tp_ptr->task_id   = rex_os_get_rtos_thread_id(p_os_tcb);
      tp_ptr->sp        = 0xdeadbeef; // not supported

      /* Signal & and Signals waiting on */
      tp_ptr->sigs      = *(rex_os_get_current_sigs_ref(p_os_tcb));
      tp_ptr->wait      = *(rex_os_get_wait_sigs_ref(p_os_tcb));

      /* Task priority */
      tp_ptr->pri      = rex_os_get_init_priority(p_os_tcb);

      /* Collect run time statistics of the task from BLAST */
      qurt_profile_get_threadid_pcycles(tp_ptr->task_id, pcycles);

      /* Collect the thread run time followed by system time
      ** so that system run time is always larger 
      */
      this_pcycle_count = qurt_get_core_pcycles();
      this_timestamp = qurt_sysclock_get_hw_ticks();

      /*  Total Pcycles */
      tp_ptr->total_samples = this_pcycle_count - rextp_pcycle_base;
      /* initialize the array */
      memset (tp_ptr->time_samples, 0, BLAST_MAX_HTHREAD_LIMIT * 8);
      /* Thread run time per HW thread */
      memscpy (tp_ptr->time_samples, QURT_MAX_HTHREADS * 8, pcycles, QURT_MAX_HTHREADS * 8);
      /* Total sleep clock ticks elapsed */
      tp_ptr->total_sclk_samples = this_timestamp - rextp_tick_base;
      /* CPU frequency */
      tp_ptr->core_clk_freq = core_clk_freq;

      log_commit(tp_ptr);
   }    
}

void rex_os_log_per_task_profiling_info(void)
{
   LOG_BLAST_TASKPROFILE_C_type *tp_ptr;
   uint64 this_pcycle_count;
   uint64 pcycles[BLAST_MAX_HTHREAD_LIMIT];
   uint32 this_timestamp;
   int index;

   /*
   ** Make sure we have plenty of time to iterate through the list
   ** of tasks and report profile information.
   */
   dog_kick();

   /* Iterate through the list of tasks and initialize their base (or start)
   ** timestamps before collecting any profiling information.
   */
   rex_task_list_mgr_for_each(rex_os_collect_tp, 0); 
   
  /* Obtain IDLE profiling */
  tp_ptr = (LOG_BLAST_TASKPROFILE_C_type *)log_alloc(LOG_BLAST_TASKPROFILE_C,
                                          sizeof(LOG_BLAST_TASKPROFILE_C_type));
   if( tp_ptr != NULL )
   {
      /* version no. */
      tp_ptr->ver = LOG_BLAST_TASKPROFILE_C_VER;
      /* App - 0 - Modem SW, 1 - Modem FW, 2 - Multi Media */
      tp_ptr->app = LOG_BLAST_TASKPROFILE_C_APP;
      /* Special task is "1" for IDLE tasks */
      tp_ptr->special_task = LOG_BLAST_TASKPROFILE_C_IDLE_TASK;
      /* Flags */
      tp_ptr->flags    = 0;
      /* Task ID */
      tp_ptr->task_id  = -1;
      tp_ptr->sp       = 0xdeadbeef; // not supported
      /* Signal & and Signals waiting on */
      tp_ptr->sigs     = 0;
      tp_ptr->wait     = 0;
      /* Task priority */
      tp_ptr->pri      = 0;

      /* Collect statitstics of the IDLE threads */
      qurt_profile_get_idle_pcycles (pcycles);

      /* get the delta idle pcycles */
      for (index=0; index<QURT_MAX_HTHREADS; index++)
      {
         pcycles[index] -= idle_bases[index];
      }

      /* Collect the thread run time followed by system time
      * so that system run time is always larger */
      this_pcycle_count = qurt_get_core_pcycles();
      this_timestamp = qurt_sysclock_get_hw_ticks();

      /*  Total Pcycles */
      tp_ptr->total_samples = this_pcycle_count - rextp_pcycle_base;
      /* initialize the array */
      memset (tp_ptr->time_samples, 0, BLAST_MAX_HTHREAD_LIMIT * 8);
      /* Thread run time per HW thread */
      memscpy (tp_ptr->time_samples, QURT_MAX_HTHREADS * 8, pcycles, QURT_MAX_HTHREADS * 8);

      /* Total sleep clock ticks elapsed */
      tp_ptr->total_sclk_samples = this_timestamp - rextp_tick_base;
      /* CPU frequency */
      tp_ptr->core_clk_freq = core_clk_freq;
         
      log_commit( tp_ptr );
   }
}
