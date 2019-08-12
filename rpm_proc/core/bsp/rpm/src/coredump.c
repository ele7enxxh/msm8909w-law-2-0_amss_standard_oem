
#include "coredump.h"
#include "HALhwio.h"
#include "rpm_hwio.h"

#include "rpm_ulog.h"
#include "swevent.h"
#include "dog.h"


extern RPM_LOG_CONTEXT rpm_ulogContext;

m3_core_dump_t rpm_core_dump =
{
    .cookie = 0xbaff1ed,

    .ulog_state = &rpm_ulogContext,
};

void notify_apss(void)
{
    HWIO_OUT(RPM_IPC, (1 << 6) | (1 << 26));
}

void err_fatal_sw_event(unsigned lr, unsigned ipsr)
{
    SWEVENT(RPM_ERR_FATAL, lr, ipsr);
}

__asm void abort(void)
{
  IMPORT rpm_core_dump
  IMPORT notify_apss
  IMPORT err_fatal_sw_event
  IMPORT dog_enable

  ; Disable all interrupts except NMI
  cpsid f
      
  ; We are going to use r0-r3 as scratch registers for a while, so save them first.
  push  {r0-r3}

  ; Get the address of the core dump structure.
  ldr   r0, =rpm_core_dump

  ; If we have already dumped registers on this run, stop here.
  ldr   r1, [r0, #4] ; rpm_core_dump.dumped_at first chunk
  cbnz  r1, hang     ; if nonzero, previous dump performed
  ldr   r1, [r0, #8] ; rpm_core_dump.dumped_at second chunk
  cbnz  r1, hang     ; if nonzero, previous dump performed

  ; Read the timetick and store it
  ldr   r1, =timer_addr
  ldr   r1, [r1]     ; r1 now points to "lo" word of timetick
retry
  ldr   r3, [r1, #4] ; read "hi" word
  ldr   r2, [r1]     ; read "lo" word
  str   r2, [r0, #4] ; save "lo" word
  ldr   r2, [r1, #4] ; read "hi" word again
  cmp   r2, r3       ; if "hi" word rolled over while we dumped
  bne   retry        ;   we must retry
  str   r2, [r0, #8] ; save "hi" word

  ; Store r4-r12
  add   r0, r0, #28 ; skip cookie, dump timestamp, and r0-r3
  stmia r0!, {r4-r12}

  ; Go back and get r0-r3
  pop   {r2-r5}
  ldr   r1, =rpm_core_dump  ; get a fresh pointer to the core dump [r0 is just past registers, we need that later]
  add   r1, r1, #12         ; move past cookie and timestamp to where r0-r3 should be stored
  stm   r1!, {r2-r5}

  ; Grab the remaining registers.
  mrs   r1, MSP
  mrs   r2, PSP
  mov   r3, lr
  ldr   r4, =abort
  mrs   r5, XPSR
  mrs   r6, PRIMASK
  mrs   r7, FAULTMASK
  mrs   r8, BASEPRI
  mrs   r9, CONTROL
  stmia r0!, {r1-r9}

  ; Grab and save all the auxiliary fault status registers
  ldr   r1, =0xE000ED24
  ldmia r1!, {r2-r8}
  stmia r0!, {r2-r8}
  
  ; Add something useful to the log
  mov   r0, lr
  mrs   r1, IPSR
  push  {r0, lr}            ; push lr so we can pop later and preserve the stack for the debugger. Need to push 8 bytes
  bl    err_fatal_sw_event
  mov   r0, #0x0
  bl    dog_enable
  
  bl    notify_apss
  
  ; Do this last
  pop   {r0, lr}

hang
  push  {r0, r1}
  ldr   r0, =0xE000EDF0
  ldr   r1, [r0]
  orr   r1, r1, #0x3
  str   r1, [r0]
  pop   {r0, r1}
  b     hang

  ALIGN 4
timer_addr
  dcd   HWIO_RPM_F0_QTMR_V1_CNTPCT_LO_ADDR 
}

