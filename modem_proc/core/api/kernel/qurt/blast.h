#ifndef BLAST_H
#define BLAST_H 

/**
  @file blast.h 
  @brief  Contain kernel header files which provides Kernel OS API functions, constants, and 
  definitions 

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ======================================================================*/
/*======================================================================
 *
 *											 EDIT HISTORY FOR FILE
 *
 *	 This section contains comments describing changes made to the
 *	 module. Notice that changes are listed in reverse chronological
 *	 order.
 *
 *	
 *
 *
 * when 				who 		what, where, why
 * ---------- 	--- 		------------------------------------------------
 * 2011-02-25 	op			Add Header file
 ======================================================================*/
 

#ifdef __cplusplus
extern "C" {
#endif

#include "qurt_timer.h"
#include "qurt.h"

#define BLAST_MAX_HTHREADS QURT_MAX_HTHREADS
#define blast_realloc        qurt_realloc
#define blast_malloc        qurt_malloc
#define blast_free        qurt_free
#define blast_calloc        qurt_calloc
#define blast_allsignal_get                 qurt_allsignal_get
#define blast_allsignal                     qurt_allsignal
#define blast_allsignal_destroy             qurt_allsignal_destroy
#define blast_allsignal_wait                qurt_allsignal_wait
#define blast_allsignal_init                qurt_allsignal_init
#define blast_allsignal_t                   qurt_allsignal_t
#define blast_allsignal_signal              qurt_allsignal_set
#define blast_anysignal                     qurt_anysignal
#define blast_anysignal_init                qurt_anysignal_init
#define blast_anysignal_t                   qurt_anysignal_t
#define blast_anysignal_destroy             qurt_anysignal_destroy
#define blast_anysignal_clear               qurt_anysignal_clear
#define blast_anysignal_wait                qurt_anysignal_wait
#define blast_anysignal_set                 qurt_anysignal_set
#define blast_anysignal_get                 qurt_anysignal_get

#define blast_atomic64_dec                  qurt_atomic64_dec
#define blast_atomic64_or                   qurt_atomic64_or
#define blast_atomic64_or_return            qurt_atomic64_or_return
#define blast_atomic_set_bit                qurt_atomic_set_bit
#define blast_atomic_inc_return             qurt_atomic_inc_return
#define blast_atomic_sub                    qurt_atomic_sub
#define blast_atomic_and_return             qurt_atomic_and_return
#define blast_atomic_xor_return             qurt_atomic_xor_return
#define blast_atomic_dec_return             qurt_atomic_dec_return
#define blast_atomic64_add_return           qurt_atomic64_add_return
#define blast_atomic64_barrier_write_smp    qurt_atomic64_barrier_write_smp
#define blast_atomic64_change_bit           qurt_atomic64_change_bit
#define blast_atomic_xor                    qurt_atomic_xor
#define blast_atomic_barrier_write_smp      qurt_atomic_barrier_write_smp
#define blast_atomic64_barrier_smp          qurt_atomic64_barrier_smp
#define blast_atomic64_barrier_write        qurt_atomic64_barrier_write
#define blast_atomic_barrier_read_smp       qurt_atomic_barrier_read_smp
#define blast_atomic64_set_bit              qurt_atomic64_set_bit
#define blast_atomic_barrier_smp            qurt_atomic_barrier_smp
#define blast_atomic_compare_and_set        qurt_atomic_compare_and_set
#define blast_atomic_sub_return             qurt_atomic_sub_return
#define blast_atomic64_set                  qurt_atomic64_set
#define blast_atomic_or                     qurt_atomic_or
#define blast_atomic_dec                    qurt_atomic_dec
#define blast_atomic64_barrier              qurt_atomic64_barrier
#define blast_atomic_barrier_read           qurt_atomic_barrier_read
#define blast_atomic_inc                    qurt_atomic_inc
#define blast_atomic_add_return             qurt_atomic_add_return
#define blast_atomic64_inc                  qurt_atomic64_inc
#define blast_atomic_barrier                qurt_atomic_barrier
#define blast_atomic64_dec_return           qurt_atomic64_dec_return
#define blast_atomic_or_return              qurt_atomic_or_return
#define blast_atomic64_add                  qurt_atomic64_add
#define blast_atomic_barrier_write          qurt_atomic_barrier_write
#define blast_atomic64_sub                  qurt_atomic64_sub
#define blast_atomic_add                    qurt_atomic_add
#define blast_atomic64_compare_and_set      qurt_atomic64_compare_and_set
#define blast_atomic64_and_return           qurt_atomic64_and_return
#define blast_atomic64_clear_bit            qurt_atomic64_clear_bit
#define blast_atomic64_barrier_read         qurt_atomic64_barrier_read
#define blast_atomic_change_bit             qurt_atomic_change_bit
#define blast_atomic64_inc_return           qurt_atomic64_inc_return
#define blast_atomic_clear_bit              qurt_atomic_clear_bit
#define blast_atomic64_xor                  qurt_atomic64_xor
#define blast_atomic64_barrier_read_smp     qurt_atomic64_barrier_read_smp
#define blast_atomic                        qurt_atomic
#define blast_atomic_set                    qurt_atomic_set
#define blast_atomic_and                    qurt_atomic_and
#define blast_atomic64_sub_return           qurt_atomic64_sub_return
#define blast_atomic64_and                  qurt_atomic64_and
#define blast_atomic64_xor_return           qurt_atomic64_xor_return

#define blast_barrier_t                     qurt_barrier_t
#define blast_barrier_wait                  qurt_barrier_wait
#define blast_barrier                       qurt_barrier
#define blast_barrier_destroy               qurt_barrier_destroy
#define blast_barrier_init                  qurt_barrier_init
 
#define BLAST_BARRIER_OTHER        QURT_BARRIER_OTHER
#define BLAST_BARRIER_SERIAL_THREAD        QURT_BARRIER_SERIAL_THREAD
#define blast_cond_init                     qurt_cond_init
#define blast_cond_destroy                  qurt_cond_destroy
#define blast_cond_signal                   qurt_cond_signal
#define blast_cond_wait                     qurt_cond_wait
#define blast_mutex_t                       qurt_mutex_t
#define blast_cond_t                        qurt_cond_t
#define blast_cond_broadcast                qurt_cond_broadcast
#define blast_get_core_pcycles                  qurt_get_core_pcycles
#define blast_profile_enable                    qurt_profile_enable
#define blast_profile_reset_thread_pcycles      qurt_profile_reset_threadid_pcycles
#define blast_profile_get_thread_pcycles        qurt_profile_get_threadid_pcycles
#define blast_profile_get_idle_pcycles          qurt_profile_get_idle_pcycles
#define blast_profile_reset_idle_pcycles        qurt_profile_reset_idle_pcycles
#define blast_get_pcycles                       qurt_profile_get_thread_pcycles
#define blast_get_tcycles                       qurt_profile_get_thread_tcycles
#define blast_power_control                     qurt_power_control
#define blast_thread_join        qurt_thread_join
#define blast_arch_version        qurt_arch_version
#define blast_arch_version_t        qurt_arch_version_t
#define blast_app_heap_obj_t        qurt_app_heap_obj_t
#define blast_get_env_object        qurt_get_env_object

#define blast_register_fastint              qurt_fastint_register
#define blast_deregister_fastint            qurt_fastint_deregister
#define blast_isr_deregister                qurt_isr_deregister
#define blast_isr_register                  qurt_isr_register

#define blast_futex_wake                    qurt_futex_wake
#define blast_futex_wait                    qurt_futex_wait

#define blast_interrupt_getconfig           qurt_interrupt_get_config
#define blast_interrupt_status              qurt_interrupt_status
#define blast_get_registered_interrupts     qurt_interrupt_get_registered
#define blast_register_interrupt            qurt_interrupt_register
#define blast_deregister_interrupt          qurt_interrupt_deregister
#define blast_interrupt_setconfig           qurt_interrupt_set_config
#define blast_interrupt_clear               qurt_interrupt_clear
#define blast_ack_interrupt                 qurt_interrupt_acknowledge

#define blast_lifo_push                     qurt_lifo_push
#define blast_lifo_pop                      qurt_lifo_pop

#define blast_addr_t                        qurt_addr_t



#define blast_remove_mapping                qurt_mapping_remove
#define blast_create_mapping                qurt_mapping_create
#define blast_mem_map_static_query          qurt_mem_map_static_query
#define blast_lookup_physaddr               qurt_lookup_physaddr

#define BLAST_EOK        QURT_EOK
#define BLAST_EVAL        QURT_EVAL
#define BLAST_MEM_CACHE_WRITEBACK        QURT_MEM_CACHE_WRITEBACK
#define BLAST_MEM_MAPPING_PHYS_CONTIGUOUS        QURT_MEM_MAPPING_PHYS_CONTIGUOUS
#define blast_mutex_lock                  qurt_mutex_lock
#define blast_mutex_trylock               qurt_mutex_try_lock
#define blast_mutex_init                  qurt_mutex_init
#define blast_mutex_destroy               qurt_mutex_destroy
#define blast_mutex_unlock                qurt_mutex_unlock
#define blast_pimutex_destroy             qurt_pimutex_destroy
#define blast_pimutex_unlock              qurt_pimutex_unlock
#define blast_pimutex                     qurt_pimutex
#define blast_pimutex_lock                qurt_pimutex_lock
#define blast_pimutex_init                qurt_pimutex_init
#define blast_pimutex_trylock             qurt_pimutex_try_lock
#define blast_pipe_send                   qurt_pipe_send
#define blast_pipe_recv                   qurt_pipe_receive
#define blast_pipe_try_recv               qurt_pipe_try_receive
#define blast_pipe_data_t                 qurt_pipe_data_t
#define blast_pipe_t                      qurt_pipe_t
#define blast_sem_t                       qurt_sem_t
#define blast_pmu_get                     qurt_pmu_get
#define blast_pmu_set                     qurt_pmu_set
#define blast_pmu_enable                  qurt_pmu_enable
#define BLAST_PMUCNT3                     QURT_PMUCNT3
#define BLAST_PMUCNT2                     QURT_PMUCNT2
#define BLAST_PMUCNT1                     QURT_PMUCNT1
#define BLAST_PMUCNT0                     QURT_PMUCNT0
#define BLAST_PMUCNT5                     QURT_PMUCNT5
#define BLAST_PMUCNT4                     QURT_PMUCNT4
#define blast_power_wait_for_idle         qurt_power_wait_for_idle
#define blast_power_tcxo_fail_exit        qurt_power_tcxo_fail_exit
#define blast_power_wait_for_active       qurt_power_wait_for_active
#define blast_system_ipend_get            qurt_system_ipend_get
#define blast_thread_wait_for_idle        qurt_power_wait_for_idle
#define blast_thread_wait_for_active      qurt_power_wait_for_active

#define blast_power_tcxo_enter            qurt_power_tcxo_enter
#define blast_power_shutdown_fail_exit    qurt_power_shutdown_fail_exit
#define blast_power_shutdown_prepare      qurt_power_shutdown_prepare
#define blast_power_tcxo_exit             qurt_power_tcxo_exit
#define blast_power_shutdown_exit         qurt_power_shutdown_exit
#define blast_power_tcxo_prepare          qurt_power_tcxo_prepare

#define blast_prio_set                    qurt_thread_set_priority
#define blast_prio_get                    qurt_thread_get_priority
#define blast_rmutex_lock                 qurt_rmutex_lock
#define blast_rmutex_unlock               qurt_rmutex_unlock
#define blast_rmutex_trylock              qurt_rmutex_try_lock
#define blast_rmutex_destroy              qurt_rmutex_destroy
#define blast_rmutex_init                 qurt_rmutex_init
#define blast_rmutex_trylock_block_once   qurt_rmutex_try_lock_block_once

#define blast_sem_add                     qurt_sem_add
#define blast_sem_init_val                qurt_sem_init_val
#define blast_sem_get_val                 qurt_sem_get_val
#define blast_sem_init                    qurt_sem_init
#define blast_sem_up                      qurt_sem_up
#define blast_sem_destroy                 qurt_sem_destroy
#define blast_sem_trydown                 qurt_sem_try_down
#define blast_sem_down                    qurt_sem_down
#define blast_thread_suspend        qurt_thread_suspend
#define blast_thread_resume        qurt_thread_resume
#define blast_thread_setattr        qurt_thread_setattr
#define blast_yield        qurt_yield
#define blast_thread_stop        qurt_thread_stop
#define blast_thread_exit        qurt_thread_exit
#define blast_cache_partition_t        qurt_cache_partition_t

#define blast_thread_get_tid        qurt_thread_get_timetest_id
#define blast_thread_set_tid        qurt_thread_set_timetest_id
#define blast_thread_myid        qurt_thread_get_id

#define blast_thread_set_hw_bitmask  qurt_thread_set_affinity
#define blast_thread_set_prefetch        qurt_thread_set_prefetch
#define BLAST_THREAD_CFG_BITMASK_ALL        QURT_THREAD_CFG_BITMASK_ALL

//not backward compatible
//#define blast_tlb_remove             qurt_tlb_remove
//#define blast_tlb_mapping_delete     qurt_tlb_entry_delete
//#define blast_tlb_mapping_create     qurt_tlb_entry_create
//#define blast_tlb_setentry           qurt_tlb_entry_set
//#define blast_tlb_getentry           qurt_tlb_entry_get
//#define blast_tlb_query              qurt_tlb_entry_query

#define blast_tls_key_delete         qurt_tls_delete_key
#define blast_tls_setspecific        qurt_tls_set_specific
#define blast_tls_getspecific        qurt_tls_get_specific
#define blast_tls_key_create         qurt_tls_create_key
/* Internal */
#define blast_tls_init               qurt_tls_init
#define BLAST_tls_reserve            QURT_tls_reserve

#define BLAST_ugp_ptr        QURT_ugp_ptr
#define BLAST_MAX_TLS        QURT_MAX_TLS
#define BLAST_MAX_TLS_INDEX        QURT_MAX_TLS_INDEX
#define BLAST_utcb_t        QURT_utcb_t
#define BLAST_UTCB        QURT_UTCB

#define blast_get_trace_marker        qurt_trace_get_marker   
#define blast_has_preempt_trace(a)    qurt_trace_changed(a,0x3)  

#define blast_perm_t        qurt_perm_t
#define BLAST_MEM_CACHE_WRITETHROUGH_L2CACHEABLE        QURT_MEM_CACHE_WRITETHROUGH_L2CACHEABLE
#define BLAST_MEM_CACHE_NONE_SHARED        QURT_MEM_CACHE_NONE_SHARED
#define BLAST_MEM_CACHE_WRITETHROUGH_NONL2CACHEABLE        QURT_MEM_CACHE_WRITETHROUGH_NONL2CACHEABLE
#define BLAST_MEM_CACHE_WRITEBACK_NONL2CACHEABLE        QURT_MEM_CACHE_WRITEBACK_NONL2CACHEABLE
#define BLAST_PERM_EXECUTE        QURT_PERM_EXECUTE
#define BLAST_MEM_CACHE_WRITEBACK_L2CACHEABLE        QURT_MEM_CACHE_WRITEBACK_L2CACHEABLE
#define BLAST_PERM_READ        QURT_PERM_READ
#define BLAST_PERM_FULL        QURT_PERM_FULL
#define BLAST_PERM_WRITE        QURT_PERM_WRITE
#define BLAST_MEM_CACHE_NONE        QURT_MEM_CACHE_NONE
 
#define blast_printf qurt_printf
#define blast_power_shutdown(a)  qurt_power_shutdown_enter((unsigned int)QURT_POWER_SHUTDOWN_TYPE_L2NORET)

#define blast_system_sclk_attr_gethwticks  qurt_sysclock_get_hw_ticks

#define blast_system_sclk_register qurt_sysclock_register
#define blast_system_sclk_alarm  qurt_sysclock_alarm_create
#define blast_system_sclk_timer  qurt_sysclock_timer_create

#define blast_reg_error_handler  qurt_exception_wait
#define blast_exit  qurt_exception_raise_nonfatal
#define blast_fatal_exit qurt_exception_raise_fatal

#define blast_mem_region_attr_t qurt_mem_region_attr_t
#define BLAST_PMUEVTCFG                   QURT_PMUEVTCFG    
#define BLAST_PMUCFG                      QURT_PMUCFG 

#define EOK                             QURT_EOK 
#define EVAL                            QURT_EVAL
#define EMEM                            QURT_EMEM
#define EINVALID                        QURT_EINVALID
#define EFAILED                         QURT_EFAILED
#define ENOTALLOWED                     QURT_ENOTALLOWED
#define EDUP_CLSID                      QURT_EDUPCLSID
#define EBADPARM                        QURT_EINVALID
#define EINVALIDITEM                    QURT_EINVALID
#define EBADHANDLE                      QURT_EINVALID
#define ENO_INTERRUPTS                  QURT_ENOREGISTERED
#define EPC_ISDB                        QURT_EISDB
#define EPC_NOSTM                       QURT_ESTM
#define E_TLS_NOAVAIL                   QURT_ETLSAVAIL  
#define E_TLS_NOENT                     QURT_ETLSENTRY   
#define EINT                            QURT_EINT   /**< Invalid interrupt number (not registered). */
#define ESIG                            QURT_ESIG   /**< Invalid signal bitmask (can't set more than one signal at a time) */
#define E_OUTOFHEAP                     QURT_EHEAP   /**< Out of heap */
#define E_MM_OUT_OF_RANGE               QURT_EMEMMAP
#define ENO_THREAD                      QURT_ENOTHREAD
#define E_L2CACHABLE_NOT_SUPPORTED      QURT_EL2CACHE
#define E_FATAL                         QURT_EFATAL    /**< FATAL error that shall never happens */
#define E_INT_DEREGISTER                QURT_EDEREGISTERED    /**< error interrupt is already de-registered*/
#define E_TLB_CREATE_SIZE               QURT_ETLBCREATESIZE   /**< TLB create error: incorrect sizes*/
#define E_TLB_CREATE_UNALIGNED          QURT_ETLBCREATEUNALIGNED    /**< TLB create error: unaligned address*/

#define EVENT_PAGEFAULT      QURT_EVENT_PAGEFAULT /**< Page fault Event */
#define EVENT_SYSTEM_ERR     QURT_EVENT_SYSTEM_ERR /**< System Error Event */
#define EVENT_SUSPEND        QURT_EVENT_SUSPEND

#define ENV_OBJ_SWAP_POOLS   QURT_ENV_OBJ_SWAP_POOLS /**< Swap spool object */
#define ENV_OBJ_APP_HEAP     QURT_ENV_OBJ_APP_HEAP /**< Application heap object */
#define ENV_OBJ_TIMER        QURT_ENV_OBJ_TIMER /**< Timer object */
#define ENV_OBJ_ARCH_VER     QURT_ENV_OBJ_ARCH_VER /**< Architecture version object */


#define PIPE_MAGIC  QURT_PIPE_MAGIC

#define MAX_QURT_SCLK_CLIENTS      QURT_SCLK_CLIENTS_MAX

#define BLAST_MAX_HTHREAD_LIMIT  QURT_MAX_HTHREAD_LIMIT

#define blast_interrupt_raise              qurt_interrupt_raise
#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif

#endif /* BLAST_H */

