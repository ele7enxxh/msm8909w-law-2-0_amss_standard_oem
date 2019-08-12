
#include <string.h>
#include "cortex-m3.h"
#include "comdef.h"
#include "rpm_hwio.h"
#include "HALhwio.h"
#include "coredump.h"
#include "image_layout.h"
#include "CoreVerify.h"
#include "swevent.h"

#define WATCHDOG_IRQ_NUM 49

void smd_isr(void) __irq;
void dog_bark_isr(void) __irq;
__weak void vmpm_isr(void) __irq;
__weak void settling_timer_isr(void) __irq;

typedef void (*ExecFuncPtr)(void) __irq;

extern void __main(void);

void abort_isr(void) __irq
{
    abort();
}

void tz_abort_isr(void) __irq 
{
    SWEVENT(RPM_TZ_HALT_INT_RECEIVED);
    abort();
}

//This needs to be aligned to at least 0x200 bytes (see arm documentation for why).
static ExecFuncPtr vector_table[16+64] __attribute__((at(0x090200))) =
{
  // RPM's standard system-level Cortex-M3 exceptions come first.
  (ExecFuncPtr)0,            				 // Initial stack pointer - not used
  (ExecFuncPtr)__main,                       // Image entry point
  abort_isr,                                 // NMI
  abort_isr,                                 // Hard fault
  abort_isr,                                 // Memory management
  abort_isr,                                 // Bus fault
  abort_isr,                                 // Usage fault
  0, 0, 0, 0,                                // Reserved
  abort_isr,                                 // SVC
  abort_isr,                                 // Debug Monitor
  0,                                         // Reserved
  abort_isr,                                 // PendSVC
  abort_isr,                                 // SysTick

  // RPM's NVIC interrupts start here.
  abort_isr,    // Krait[0] shutdown_req
  abort_isr,    // Krait[0] bringup_req
  abort_isr,    // Krait[1] shutdown_req
  abort_isr,    // Krait[1] bringup_req
  abort_isr,    // Krait[2] shutdown_req
  abort_isr,    // Krait[2] bringup_req
  abort_isr,    // Krait[3] shutdown_req
  abort_isr,    // Krait[3] bringup_req
  smd_isr,      // Apps HLOS SMD interrupt
  vmpm_isr,     // Apps HLOS VMPM interrupt
  abort_isr,    // Apps HLOS reserved interrupt
  smd_isr,      // TZ SMD interrupt
  tz_abort_isr, // TZ HALT interrupt
  abort_isr,    // TZ reserved interrupt
  abort_isr,    // Apps L2 Cache shutdown_req
  abort_isr,    // Apps L2 Cache bringup_req
  abort_isr,    // Krait dbgCorePwrUpInt
  abort_isr,    // <spare / reserved>
  abort_isr,    // reserved
  abort_isr,    // reserved
  abort_isr,    // reserved
  abort_isr,    // reserved
  abort_isr,    // reserved
  abort_isr,    // <spare / reserved>
  abort_isr,    // MPSS shutdown_req
  abort_isr,    // MPSS bringup_req
  smd_isr,      // MPSS SMD interrupt
  vmpm_isr,     // MPSS VMPM interrupt
  abort_isr,    // MPSS reserved interrupt
  abort_isr,    // top_cpr_irq[2]
  abort_isr,    // Pronto shutdown_req
  abort_isr,    // Pronto bringup_req
  smd_isr,      // Pronto SMD interrupt
  vmpm_isr,     // Pronto VMPM interrupt
  abort_isr,    // Pronto reserved interrupt
  abort_isr,    // qdss_clk_irq
  abort_isr,    // QDSS dbgSysPwrUpReq assertion
  abort_isr,    // QDSS dbgSysPwrUpReq de-assertion
  abort_isr,    // mmss_summary_intr
  abort_isr,    // tlmm_summary_intr
  abort_isr,    // tlmm_dir_conn_intr
  abort_isr,    // tsense_upper_lower_intr
  abort_isr,    // MPM summary interrupt
  abort_isr,    // mmss_cpr_irq[2]
  abort_isr,    // SPDM Realtime IRQ
  abort_isr,    // ce_core_irq
  abort_isr,    // ce_bam_irq
  abort_isr,    // SPMI command done
  abort_isr,    // SPMI peripheral IRQ (a.k.a. PMIC interrupt)
  dog_bark_isr, // WDOG bark IRQ
  abort_isr,    // QTimer[0] IRQ
  settling_timer_isr,    // QTimer[1] IRQ
  abort_isr,    // Debug CTI[0]
  abort_isr,    // Debug CTI[1]
  abort_isr,    // LPASS shutdown_req
  abort_isr,    // LPASS bringup_req
  smd_isr,      // LPASS SMD interrupt
  vmpm_isr,     // LPASS VMPM interrupt
  abort_isr,    // LPASS reserved interrupt
  abort_isr,    // bus_timeout_irq
  abort_isr,    // mmss_cpr_irq[0]
  abort_isr,    // mmss_cpr_irq[1]
  abort_isr,    // top_cpr_irq[0]
  abort_isr,    // top_cpr_irq[1]
};

void exceptions_init(void)
{
  int i, num_ints;

  // Disable interrupts globally.
  __disable_irq();

  // Default all RPM interrupts to rising edge.
  HWIO_OUT(RPM_INTR_POLARITY_0, 0xFFFFFFFF);
  HWIO_OUT(RPM_INTR_POLARITY_1, 0xFFFFFFFF);
  HWIO_OUT(RPM_INTR_EDG_LVL_0,  0xFFFFFFFF);
  HWIO_OUT(RPM_INTR_EDG_LVL_1,  0xFFFFFFFF);

  // Figure out how many interrupts are supported on this platform.
  num_ints = ((SCS.IntCtrlType & 0x1F) + 1) << 5;

  // Disable all interrupts.
  for(i = 0; i < num_ints / 32; ++i)
    SCS.NVIC.Disable[i] = 0xFFFFFFFF;

  // Clear all interrupts.
  for(i = 0; i < num_ints / 32; ++i)
    SCS.NVIC.Clear[i] = 0xFFFFFFFF;

  // Set all interrupt levels as pre-empting.
  SCS.AIRC = 0x05FA0000;

  // based on the size of vector_table and method of vector_table indexing (i (upto < num_ints) + 16)
  CORE_VERIFY( num_ints <= 64 );
  
  // Set all interrupts to lowest priority.
  for(i = 0; i < num_ints / 4; ++i)
    SCS.NVIC.Priority[i] = 0xFFFFFFFF;

  // Configure the exception vector table.
  SCS.ExceptionTableOffset = (unsigned)&vector_table;

  // Enable traps on divides by 0 and double-word stack alignment.
  SCS.ConfigCtrl |= 0x210;

  // Memory/Bus/Usage faults get highest user priority.
  SCS.SystemPriority[0] = 0;

  // Other system exceptions get middling priority.
  SCS.SystemPriority[1] = 0xC0000000; // SVC = 0xC0
  SCS.SystemPriority[2] = 0xFFFF0080; // SysTick and PendSV = 0xFF, Debug = 0x80

  // Enable the usage, bus, and memory management fault handlers.
  SCS.SystemHandlerCtrlAndState = 0x70000;

  // Now that we're all set, go through and re-enable interrupts that have handlers.
  for(i = 0; i < num_ints; ++i)
  {
    // Check for weakly linked handler that doesn't exist yet.
    // Note the +16 that skips system handlers.
    if(!vector_table[i + 16])
        vector_table[i + 16] = abort_isr;

    if(abort_isr != vector_table[i + 16])
      SCS.NVIC.Enable[i/32] = (1 << (i % 32));
  }

  // Give the watchdog bark IRQ the same priority as a fault.
  interrupt_set_priority(WATCHDOG_IRQ_NUM, 0);

  // Enable fault handling.
  set_basepri(0);
  INTLOCK();
#if (!defined(MPQ8092_STUBS))
  __enable_irq();
#endif
}

void exceptions_enable(void)
{
    INTFREE();
}

void interrupt_soft_trigger(unsigned num)
{
    CORE_VERIFY(num < 64);
    STIR.SoftwareTriggerInterrupt = num;
}

void interrupt_set_isr(unsigned num, isr_type isr)
{
    vector_table[16+num] = isr;
}

void interrupt_configure(unsigned num, interrupt_config config)
{
    volatile unsigned *RPM_INTERRUPT_POLARITY = (unsigned int *)HWIO_RPM_INTR_POLARITY_0_ADDR;
    volatile unsigned *RPM_INTERRUPT_EDGE = (unsigned int *)HWIO_RPM_INTR_EDG_LVL_0_ADDR;

    switch(config)
    {
        case RISING_EDGE:
            RPM_INTERRUPT_POLARITY[num/32] |=  (1 << (num % 32));
            RPM_INTERRUPT_EDGE[num/32]     |=  (1 << (num % 32));
            break;

        case FALLING_EDGE:
            RPM_INTERRUPT_POLARITY[num/32] &= ~(1 << (num % 32));
            RPM_INTERRUPT_EDGE[num/32]     |=  (1 << (num % 32));
            break;

        case LEVEL_HIGH:
            RPM_INTERRUPT_POLARITY[num/32] |=  (1 << (num % 32));
            RPM_INTERRUPT_EDGE[num/32]     &= ~(1 << (num % 32));
            break;

        case LEVEL_LOW:
            RPM_INTERRUPT_POLARITY[num/32] &= ~(1 << (num % 32));
            RPM_INTERRUPT_EDGE[num/32]     &= ~(1 << (num % 32));
            break;
    }
}

void interrupt_set_priority(unsigned num, unsigned priority)
{
    unsigned priority_word = SCS.NVIC.Priority[num/4];
    unsigned priority_offset = 8 * (num % 4);
    unsigned priority_mask = ~(0xff << priority_offset);

    SCS.NVIC.Priority[num/4] = (priority_word & priority_mask) | ((0xff & priority) << priority_offset);
}

unsigned intlock_nest_level = 0;

void lock_ints(void)
{
  unsigned _was_masked = get_basepri();
  set_basepri(0x10);
  assert(!intlock_nest_level || _was_masked);
  ++intlock_nest_level;
}

void unlock_ints(void)
{
  unsigned _is_masked = get_basepri();
  assert(intlock_nest_level && _is_masked);
  if(!--intlock_nest_level)
  {
    set_basepri(0);
  }
}

/*===========================================================================
FUNCTION: is_any_interrupt_pending

DESCRIPTION: 
ARM TRM says that ISRPENDING indicates a pending interrupt irrespective
of whether it is enabled or not. So This cannot be used for checking.
VECTPENDING gives the highest priority pending exception, but it also includes the 
effect of the BASEPRI and FAULTMASK registers, but not any effect of the PRIMASK register. 
We’re in an intlocked context, and we use BASEPRI as our intlock mechanism. 
BASEPRI will be 16, and thus VECTPENDING will effectively mask out all of the other interrupts.
Instead we check NVIC Enabled and Set registers to understand whether interrupts
are pending.

RETURN VALUE: True if any interrupt is pending
===========================================================================*/
bool is_any_interrupt_pending(void)
{
  int num_ints, i;
  bool result = false;
  num_ints = ((SCS.IntCtrlType & 0x1F) + 1) << 5;
  for(i = 0; ((i < num_ints / 32) && !result); i++)
  {
    if (SCS.NVIC.Enable[i] & SCS.NVIC.Set[i])
      result = true;
  }
  return result;
}
