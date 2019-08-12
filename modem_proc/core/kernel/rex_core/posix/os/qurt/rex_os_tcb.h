#ifndef __REX_OS_TCB_H__
#define __REX_OS_TCB_H__

#if defined(FEATURE_QURT)
   typedef qurt_mutex_t             REX_OS_LOCK_TYPE;
   #define REX_OS_MUTEX_INIT        qurt_pimutex_init
   #define REX_OS_MUTEX_LOCK        qurt_pimutex_lock
   #define REX_OS_MUTEX_TRY_LOCK    qurt_pimutex_try_lock
   #define REX_OS_MUTEX_UNLOCK      qurt_pimutex_unlock
   #define REX_OS_MUTEX_DELETE      qurt_pimutex_destroy
   #define REX_OS_GET_RTOS_TID      qurt_thread_get_id
#elif defined(FEATURE_BLAST)
   typedef blast_mutex_t            REX_OS_LOCK_TYPE;
   #define REX_OS_MUTEX_INIT        blast_pimutex_init
   #define REX_OS_MUTEX_LOCK        blast_pimutex_lock
   #define REX_OS_MUTEX_TRY_LOCK    blast_pimutex_trylock
   #define REX_OS_MUTEX_UNLOCK      blast_pimutex_unlock
   #define REX_OS_MUTEX_DELETE      blast_pimutex_destroy
   #define REX_OS_GET_RTOS_TID      blast_thread_myid
#else
   #error "Unknown OS configuration"
#endif

INLINE 
int rex_os_get_rtos_thread_id(rex_os_tcb_type *p_os_tcb)
{
   return p_os_tcb->q6_rtos_id;
}

INLINE 
void rex_os_set_rtos_thread_id(rex_os_tcb_type *p_os_tcb, int id)
{
   p_os_tcb->q6_rtos_id = id;
}

INLINE 
void rex_os_set_tp_tick_base(rex_os_tcb_type *p_os_tcb, uint32 tt)
{
   p_os_tcb->rextp_tick_base = tt;
}

INLINE 
uint32 rex_os_get_tp_tick_base(rex_os_tcb_type *p_os_tcb)
{
   return p_os_tcb->rextp_tick_base;
}

INLINE
void rex_tcb_os_specific_init(rex_os_tcb_type *p_os_tcb)
{
   rex_os_set_rtos_thread_id(p_os_tcb, 0);
   rex_os_set_tp_tick_base(p_os_tcb, 0);
}

INLINE
void rex_tcb_os_specific_init_attr(rex_os_tcb_type *p_os_tcb, rex_task_attributes_type *p_attr)
{

}

INLINE
void rex_tcb_os_specific_delete(rex_os_tcb_type *p_os_tcb)
{

}

#endif //__REX_OS_TCB_H__
