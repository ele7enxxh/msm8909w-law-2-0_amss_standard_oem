/**
@file tzbsp_isr.c
@brief Registered ISR array and the related register/unregister/dispatch
       functions.
*/
/*===========================================================================
   Copyright (c) 2010-2012 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/kernel/src/A7SS/tzbsp_isr_array.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---     ------------------------------------------------------------
10/04/11   mm      Log Interrupt information in Diag Area
01/03/11   tk      Initial version.
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include <string.h>
#include <HALqgic.h>

#include "tzbsp.h"
#include "tzbsp_errno.h"
#include "tzbsp_sys.h"
#include "tzbsp_target.h"
#include "tzbsp_isr.h"
#include "tzbsp_config.h"
#include "tzbsp_log.h"
#include "tzbsp_diag.h"

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef struct tzbsp_int_s {
  boolean enabled;
  boolean triggerEdge;
  boolean secure;
  uint32  priority;
  uint32  targets;
} tzbsp_int_t;

typedef struct tzbsp_isr_s
{
  uint32 intnum; /* Interrupt number trapped by the registered ISR. */
  uint16 flags; /* Flags. */
  uint8  cfg_ref; /* Reference counting per CPU. */
  uint8  en_ref; /* Enable/disable reference counting per CPU. */
  void* (*fn)(void*); /* The actual interrupt service routine (ISR). */
  void* ctx; /* Optional contex for the ISR. */
  tzbsp_int_t *previous_int;
} tzbsp_isr_t;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/* The number of currently installed ISR functions in the ISR
 * array. */
static uint32 g_tzbsp_isr_count;

/* Array for registering ISRs. The array entries are in ascending
 * order based on the interrupt number. The array stores only one ISR
 * per interrupt number. */
static tzbsp_isr_t g_isrs[TZBSP_MAX_ISR_COUNT];

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

void tzbsp_config_qgic_int(uint32 intnum, uint32 target, uint32 flags,
                           boolean enable);
void tzbsp_disable_qgic_int(uint32 intnum);
void tzbsp_enable_qgic_int(uint32 intnum);
void tzbsp_set_non_secure_qgic_int(uint32 intnum);
void tzbsp_save_qgic_int(uint32   intnum,       boolean *pEnabled,
                         boolean *pTriggerEdge, boolean *pSecure,
                         uint32  *pPriority,    uint32  *pTargets);
void tzbsp_restore_qgic_int(uint32  intnum,      boolean enabled,
                            boolean triggerEdge, boolean secure,
                            uint32 priority,     uint32 targets);
uint32 tzbsp_cur_cluster_num(void);

extern tzbsp_mutex_t g_tzbsp_fiq_conf_mutex;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

static uint8 add_ref(uint8 flags)
{
  return flags | (1 << tzbsp_cur_cpu_num());
}

static uint8 remove_ref(uint8 flags)
{
  return flags & ~(1 << tzbsp_cur_cpu_num());
}

static void add_refs(tzbsp_isr_t* isr, uint32 flags, boolean enable)
{
  if(flags & TZBSP_INTF_ALL_CPUS)
  {
    isr->cfg_ref = 0xff;
  }
  else
  {
    isr->cfg_ref = add_ref(isr->cfg_ref);
  }

  if(enable)
  {
    if(flags & TZBSP_INTF_ALL_CPUS)
    {
      isr->en_ref = 0xff;
    }
    else
    {
      isr->en_ref = add_ref(isr->en_ref);
    }
  }
}

/* Binary searches for the ISR registered for an interrupt number. The
 * array is in ascending order based on interrupt number.
 *
 * @param [in] intnum  Interrupt number of the registered ISR.
 *
 * @return -1 if no ISR is registered for the searched interrupt number,
 *         a valid array index number otherwise.
 */
static int tzbsp_find_isr_slot(uint32 intnum)
{
  int pivot, min = 0;
  int max = g_tzbsp_isr_count - 1;

  while(min <= max)
  {
    if(intnum == g_isrs[min].intnum)
    {
      return min;
    }
    else if(intnum == g_isrs[max].intnum)
    {
      return max;
    }

    pivot = min + (max - min) / 2;

    if(intnum < g_isrs[pivot].intnum)
    {
      max = pivot;
      min++;
    }
    else
    {
      min = pivot;
      max--;
    }
  }

  return -1;
}

/** @copydoc tzbsp_save_int */
tzbsp_int_t* tzbsp_save_int(uint32 intnum)
{
  tzbsp_int_t *saved_int = (tzbsp_int_t*)tzbsp_malloc(sizeof(tzbsp_int_t));
  if (saved_int == NULL)
    return NULL;
  
  tzbsp_save_qgic_int(intnum,&saved_int->enabled,&saved_int->triggerEdge,
                      &saved_int->secure,&saved_int->priority,
                      &saved_int->targets);

  return saved_int;
}

/** @copydoc tzbsp_restore_int */
void tzbsp_restore_int(uint32 intnum, tzbsp_int_t *saved_int)
{
  tzbsp_restore_qgic_int(intnum, saved_int->enabled, saved_int->triggerEdge,
                         saved_int->secure, saved_int->priority,
                         saved_int->targets);
}

/** @copydoc tzbsp_register_isr */
int tzbsp_register_isr(uint32 intnum, const char *int_desc,
                       void* (*fn)(void*), void* ctx, uint32 flags,
                       boolean enable)
{
  int err, i;
  uint32 state = tzbsp_int_disable_all();
  uint32 target;
  
  tzbsp_mutex_lock(&g_tzbsp_fiq_conf_mutex);

  /* Determine the targeting for this interrupt */
  if(flags & TZBSP_INTF_CPU0_ONLY)
  {
    target = TZBSP_INT_TARGET_CPU(0);
  }
  else if(flags & TZBSP_INTF_ALL_CPUS)
  {
    target = TZBSP_INT_TARGET_ALL;
  }
  else
  {
    target = TZBSP_INT_TARGET_SELF;
  }

  i = tzbsp_find_isr_slot(intnum);
  if(-1 != i) /* An existing ISR found, replace it. */
  {
    /* Check which OS this ISR is registered for and make sure
     * an overwrite is allowed. Can only over write interrupts
     * of the same OS */
    if ((flags & TZBSP_INTF_SAVE_INT) && (g_isrs[i].previous_int))
    {
      /* Current interrupt configuration cannot be saved since there is a
       * previously saved configuration */
      err = -1;
    }
    else if((g_isrs[i].flags & TZBSP_INTF_INT_FLAG_EXT) ==
       (flags           & TZBSP_INTF_INT_FLAG_EXT))
    {
      g_isrs[i].intnum   = intnum;
      g_isrs[i].flags    = (uint16)flags;
      g_isrs[i].fn       = fn;
      g_isrs[i].ctx      = ctx;

      add_refs(&g_isrs[i], flags, enable);

      /* Save current interrupt configuration */
      if (flags & TZBSP_INTF_SAVE_INT)
      {
        g_isrs[i].previous_int = tzbsp_save_int(intnum);
      }

      if ((flags & TZBSP_INTF_SAVE_INT) && (NULL == g_isrs[i].previous_int))
      {
        err = -1;
      }
      else
      {
        /* Make sure the related interrupt is enabled. By default, registered
         * interrupts target the calling CPU. */
        if(0 == (flags & TZBSP_INTF_SKIP_INT_CONFIG))
        {
          tzbsp_config_qgic_int(intnum, target, flags, enable);
        }

        err = 0;
      }
    }
    else
    {
      /* Interrupt is already registered for the other OS */
      err = -1;
    }
  }
  else if(TZBSP_MAX_ISR_COUNT > g_tzbsp_isr_count)
  {
    /* No existing ISR was found, an empty slot is needed and there is
     * at least one available. */

    /* ISRs are inserted in ascending order. */
    for(i=0; i < g_tzbsp_isr_count; i++)
    {
      if(intnum < g_isrs[i].intnum)
      {
        break;
      }
    }

    /* Shift the rest of the array, if any, to free the slot. */
    if(i < g_tzbsp_isr_count)
    {
      memmove(&g_isrs[i+1], &g_isrs[i],
              sizeof(g_isrs[0]) * (g_tzbsp_isr_count - i));
    }

    g_isrs[i].cfg_ref  = 0; /* Clear for first time use. */
    g_isrs[i].en_ref   = 0; /* Clear for first time use. */

    g_isrs[i].intnum   = intnum;
    g_isrs[i].flags    = (uint16)flags;
    g_isrs[i].fn       = fn;
    g_isrs[i].ctx      = ctx;

    add_refs(&g_isrs[i], flags, enable);

    /* Update the diag information for the interrupt */
    TZBSP_DIAG_INT_ADD(intnum, int_desc);
    
    /* Save current interrupt configuration */
    if (flags & TZBSP_INTF_SAVE_INT)
    {
      g_isrs[i].previous_int = tzbsp_save_int(intnum);
    }

    if ((flags & TZBSP_INTF_SAVE_INT) && (NULL == g_isrs[i].previous_int))
    {
      err = -1;
    }
    else
    {

      /* Finally enable the actual interrupt. */
      if(0 == (flags & TZBSP_INTF_SKIP_INT_CONFIG))
      {
        tzbsp_config_qgic_int(intnum, target, flags, enable);
      }

      g_tzbsp_isr_count++;
      err = 0;
    }
  }
  else /* Empty slots were not available. */
  {
    err = -1;
  }

  tzbsp_dsb();

  tzbsp_mutex_unlock(&g_tzbsp_fiq_conf_mutex);
  tzbsp_int_restore(state);
  return err;
}

/** @copydoc tzbsp_unregister_isr */
int tzbsp_unregister_isr(uint32 intnum, uint32 flags)
{
  int err = -1;
  uint32 state = tzbsp_int_disable_all();
  tzbsp_mutex_lock(&g_tzbsp_fiq_conf_mutex);

  int i = tzbsp_find_isr_slot(intnum);

  if(-1 != i)
  {
    /* Ensure that the calling OS has permission to remove
     * the interrupt in question */
    if((g_isrs[i].flags & TZBSP_INTF_INT_FLAG_EXT) ==
       (flags           & TZBSP_INTF_INT_FLAG_EXT))
    {
      tzbsp_disable_qgic_int(g_isrs[i].intnum);

      if(flags & TZBSP_INTF_INT_FLAG_NON_SECURE)
      {
        tzbsp_set_non_secure_qgic_int(intnum);
      }

      if (flags & TZBSP_INTF_ALL_CPUS) {
        g_isrs[i].cfg_ref = 0;
        g_isrs[i].en_ref  = 0;
      } else {
      g_isrs[i].cfg_ref = remove_ref(g_isrs[i].cfg_ref);
      g_isrs[i].en_ref = remove_ref(g_isrs[i].en_ref);
      }

      if(0 == g_isrs[i].cfg_ref)
      {
        g_isrs[i].intnum  = 0;
        g_isrs[i].flags   = 0;
        g_isrs[i].cfg_ref = 0;
        g_isrs[i].en_ref  = 0;
        g_isrs[i].fn      = NULL;
        g_isrs[i].ctx     = NULL;

        if(g_isrs[i].previous_int)
        {
          tzbsp_restore_int(intnum,g_isrs[i].previous_int);
          tzbsp_free(g_isrs[i].previous_int);
          g_isrs[i].previous_int = NULL;
        }

        /* Shift the rest of the array, if any, to fill the gap. */
        if(i < (g_tzbsp_isr_count - 1))
        {
          memmove(&g_isrs[i], &g_isrs[i+1],
                  sizeof(g_isrs[0]) * (g_tzbsp_isr_count - i - 1));
        }

        g_tzbsp_isr_count--;
      }
      TZBSP_DIAG_INT_FREE(intnum);
      err = 0;
    }
  }

  tzbsp_dsb();

  tzbsp_mutex_unlock(&g_tzbsp_fiq_conf_mutex);
  tzbsp_int_restore(state);
  return err;
}

/** @copydoc tzbsp_dispatch_int */
uint32 tzbsp_dispatch_int(uint32 intnum)
{
  void* (*fn)(void*) = NULL; 
  void* ctx = NULL;
  uint32 state;
  uint32 isr_found = 0;

  state = tzbsp_int_disable_all();
  tzbsp_mutex_lock(&g_tzbsp_fiq_conf_mutex);

  int i = tzbsp_find_isr_slot(intnum);

  if(-1 != i)
  {
    TZBSP_DIAG_INCREMENT_INT_COUNT(intnum);

    ctx = g_isrs[i].ctx;
    fn = g_isrs[i].fn;
    isr_found = 1;
  }
  else
  {
    /* Any stray interrupt is just ignored. */
    tzbsp_log(TZBSP_MSG_DEBUG, "isr: Stray FIQ: %u", intnum);
  }

  tzbsp_mutex_unlock(&g_tzbsp_fiq_conf_mutex);
  tzbsp_int_restore(state);

  /*Cannot hold the lock while invoking ISR for a couple of reasons
   *1. will increase interrupt service latency if other core is running ISR
   *2. More importantly, in case of fatal error cases, only one core will be 
   *   able to enter fatal ISR (either wdog or SGI15 on one core. 
   */
  if(isr_found)
  {
    /*Invoke the ISR*/
    ctx = (*fn)(ctx);
  } 
  else
  {
    return 0;
  }

  state = tzbsp_int_disable_all();
  tzbsp_mutex_lock(&g_tzbsp_fiq_conf_mutex);

  /*Need to find the slot again, just in case it is changed*/
  i = tzbsp_find_isr_slot(intnum);

  if(-1 != i)
  {
    g_isrs[i].ctx = ctx;
  }

  tzbsp_mutex_unlock(&g_tzbsp_fiq_conf_mutex);
  tzbsp_int_restore(state);

  return 0;
}

/** @copydoc tzbsp_config_int */
void tzbsp_config_int(uint32 intnum, uint32 target, uint32 flags,
                      boolean enable)
{
  uint32 state;

  state = tzbsp_int_disable_all();
  tzbsp_mutex_lock(&g_tzbsp_fiq_conf_mutex);

  int i = tzbsp_find_isr_slot(intnum);

  if(-1 != i)
  {
    g_isrs[i].cfg_ref  = add_ref(g_isrs[i].cfg_ref);
    if(enable)
    {
      g_isrs[i].en_ref  = add_ref(g_isrs[i].en_ref);
    }
    else
    {
      g_isrs[i].en_ref  = remove_ref(g_isrs[i].en_ref);
    }
  }

  tzbsp_config_qgic_int(intnum, target, flags, enable);

  tzbsp_dsb();

  tzbsp_mutex_unlock(&g_tzbsp_fiq_conf_mutex);
  tzbsp_int_restore(state);
}

/** @copydoc tzbsp_disable_int */
void tzbsp_disable_int(uint32 intnum)
{
  uint32 state; 

  state = tzbsp_int_disable_all();
  tzbsp_mutex_lock(&g_tzbsp_fiq_conf_mutex);

  int i = tzbsp_find_isr_slot(intnum);

  if(-1 != i) /* If ISR found, keep settings in sync. */
  {
    g_isrs[i].en_ref = remove_ref(g_isrs[i].en_ref);
  }

  tzbsp_disable_qgic_int(intnum);

  tzbsp_dsb();

  tzbsp_mutex_unlock(&g_tzbsp_fiq_conf_mutex);
  tzbsp_int_restore(state);
}

/** @copydoc tzbsp_enable_int */
void tzbsp_enable_int(uint32 intnum)
{
  uint32 state; 

  state = tzbsp_int_disable_all();
  tzbsp_mutex_lock(&g_tzbsp_fiq_conf_mutex);

  int i = tzbsp_find_isr_slot(intnum);

  if(-1 != i) /* If ISR found, keep settings in sync. */
  {
    g_isrs[i].en_ref = add_ref(g_isrs[i].en_ref);
  }

  tzbsp_enable_qgic_int(intnum);

  tzbsp_dsb();

  tzbsp_mutex_unlock(&g_tzbsp_fiq_conf_mutex);
  tzbsp_int_restore(state);
}

/** @copydoc tzbsp_sec_interrupts_remove_target */
/** This routine removes the cpu from the secure interrupts' target list 
 * IMPORTANT: This Routine needs to be called from within boot lock 
 */
void tzbsp_sec_interrupts_remove_target(uint32 cpu_num)
{
  int i;
  uint32 targets;
  uint32 mask;

  
  mask = tzbsp_int_disable_all();
  tzbsp_mutex_lock(&g_tzbsp_fiq_conf_mutex);

  for(i=0; i < g_tzbsp_isr_count; i++)
  {
    if(g_isrs[i].intnum >= QGIC_SPI_INTERRUPT_BASE_NUM)
    {
      HAL_qgic_GetTargets(g_isrs[i].intnum, &targets);

      if(TZBSP_MSM_8x36 != tzbsp_get_chip_id())
      {
        targets = targets & (~(1 << cpu_num));
      }
      else /* qGIC consider cluster 0 as Cluster 1 for 8936 */
      {
        targets = targets & (~(1 << (cpu_num + TZBSP_CPU_COUNT_PER_CLUSTER)));
      }
      
      HAL_qgic_SetTargets(g_isrs[i].intnum, targets);
    }
  }

  tzbsp_dsb();

  tzbsp_mutex_unlock(&g_tzbsp_fiq_conf_mutex);
  tzbsp_int_restore(mask);
}

/** @copydoc tzbsp_sec_interrupts_add_target */
/** This routine adds the cpu to the secure interrupts' target list 
 * IMPORTANT: This Routine needs to be called from within boot lock  
 */
void tzbsp_sec_interrupts_add_target(uint32 cpu_num)
{
  int i;
  uint32 targets;
  uint32 mask;
  
  mask = tzbsp_int_disable_all();
  tzbsp_mutex_lock(&g_tzbsp_fiq_conf_mutex);

  for(i=0; i < g_tzbsp_isr_count; i++)
  {
    if(g_isrs[i].intnum >= QGIC_SPI_INTERRUPT_BASE_NUM)
    {
      HAL_qgic_GetTargets(g_isrs[i].intnum, &targets);

      if(TZBSP_MSM_8x36 != tzbsp_get_chip_id())
      {
        targets = targets | (1 << cpu_num);
      }
      else /* qGIC consider cluster 0 as Cluster 1 for 8936 */
      {
        targets = targets | (1 << (cpu_num + TZBSP_CPU_COUNT_PER_CLUSTER));
      }

      HAL_qgic_SetTargets(g_isrs[i].intnum, targets);
    }
  }

  tzbsp_dsb();

  tzbsp_mutex_unlock(&g_tzbsp_fiq_conf_mutex);
  tzbsp_int_restore(mask);
}
/* This routine checks if the core is interruptible or not.
 * If core is un-interruptible then check if this is the last 
 * core powering down, then need to make sure there is at least 
 * one other interruptible core before making this core un-interruptible.
 * If there is no other core iterruptible then this last core 
 * has to be interruptible. 
 * If the core becomes un-interruptible, this routine removes the core 
 * from the secure interrupts' target list 
 * IMPORTANT: This Routine needs to be called from within boot lock 
 */
extern uint32 tzbsp_cpus_per_cluster_in_lpm[];
uint32 g_tzbsp_interruptible_primary_cpu;
uint8 tzbsp_interruptible_core_mask[TZBSP_CPU_COUNT_MAX]; /*Need to be 0's for cold boot*/
void tzbsp_check_core_remove_int_target(uint32 interruptble, uint32 cpu_num)
{
  int i;
  uint32 cpus_in_lpm = 0;
  int any_core_interruptible = 0;

  for (i = 0; i < g_tzbsp_cluster_count; i++)
  {
    cpus_in_lpm += tzbsp_cpus_per_cluster_in_lpm[i];
  }

  if(!interruptble)
  {
    tzbsp_interruptible_core_mask[cpu_num] = 0x1;
  }
  else 
  {
    /*Hint from HLOS is that the core is un-interruptible*/
    if(cpus_in_lpm != g_tzbsp_cpu_count)
    {
      /*This is not the last core powering down. Make it un-interruptible, as hinted*/
      tzbsp_sec_interrupts_remove_target(cpu_num);
      tzbsp_interruptible_core_mask[cpu_num] = 0;
    }
    else
    {
      /*This is the last core powering down*/
      for(i = 0; i < g_tzbsp_cpu_count; i++)
      {
        if((i != cpu_num) && (tzbsp_interruptible_core_mask[i]))
        {
          any_core_interruptible = 1;
          break;
        }
      }

      if(any_core_interruptible)
      {
        /*some core is interruptible, make this core un-interruptible.*/
        tzbsp_sec_interrupts_remove_target(cpu_num);
        tzbsp_interruptible_core_mask[cpu_num] = 0;
      } 
      else 
      {
        /*No other core is interruptible and this is last core 
         *going down, cant make it un-interruptible.
         */
        tzbsp_interruptible_core_mask[cpu_num] = 1;
      }
    }
  }

  /* Pick up the core with least cpu id in the cluster 
   * that is interruptible as a primary core. Primary core 
   * information may be used for various reasions on different 
   * chipsets.  
   */
  if(cpus_in_lpm == g_tzbsp_cpu_count)
  {
    for(i = 0; i < g_tzbsp_cpu_count; i++)
    {
      if(tzbsp_interruptible_core_mask[cpu_num] == 1)
      {
        g_tzbsp_interruptible_primary_cpu = i;
        break;
      }
    }
  }
}
