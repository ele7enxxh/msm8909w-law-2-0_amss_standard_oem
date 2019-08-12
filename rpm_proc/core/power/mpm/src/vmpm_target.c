#include "vmpmi.h"
#include "vmpm.h"
#include "HALhwio.h"
#include "HALmpm.h"
#include "rpm_hwio.h"

/* functions to send interrupts to the various masters */
void send_apss_vmpm_interrupt(void)
{
  HWIO_OUT(RPM_IPC, (1<<5));
}

void send_lpass_vmpm_interrupt(void)
{
  HWIO_OUT(RPM_IPC, (1<<9));
}

void send_mpss_vmpm_interrupt(void)
{
  HWIO_OUT(RPM_IPC, (1<<13));
}

void send_pronto_vmpm_interrupt(void)
{
  HWIO_OUT(RPM_IPC, (1<<17));
}

void send_apss_wakeup_interrupt(void)
{
  HWIO_OUT(RPM_IPC, (1<<7));
}

void send_lpass_wakeup_interrupt(void)
{
  HWIO_OUT(RPM_IPC, (1<<11));
}

void send_mpss_wakeup_interrupt(void)
{
  HWIO_OUT(RPM_IPC, (1<<15));
}

void send_pronto_wakeup_interrupt(void)
{
  HWIO_OUT(RPM_IPC, (1<<19));
}


/* these addresses map to the end of the master's request section for enable
 * and clear, and to the end of the master's ack section for status
 *
 * FIXME: no need to tablize this, these addresses are all easily
 * calculabe--fix that at some point... */
const vmpm_master_info_type vmpm_masters_info[VMPM_MAX_MASTERS] =
{
  /* APSS */
  { (uint32 *) 0x600601D8, /* enable request */
    (uint32 *) 0x600601E0, /* detect ctl request */
    (uint32 *) 0x600601E8, /* detect ctl request 3*/
    (uint32 *) 0x600601F0, /* polarity request */
    (uint32 *) 0x600601F8, /* status */
    0x4,
    send_apss_wakeup_interrupt,
    send_apss_vmpm_interrupt,
    APPS_IRQ },

  /* MSS */
  { (uint32 *) 0x600611D8, /* enable request */
    (uint32 *) 0x600611E0, /* detect ctl request */
    (uint32 *) 0x600611E8, /* detect ctl request 3 */
    (uint32 *) 0x600611F0, /* polarity request */
    (uint32 *) 0x600611F8, /* status */
    0x5,
    send_mpss_wakeup_interrupt,
    send_mpss_vmpm_interrupt,
    MPSS_IRQ },

  /* PRONTO */
  { (uint32 *) 0x600621D8, /* enable request */
    (uint32 *) 0x600621E0, /* detect ctl request */
    (uint32 *) 0x600621E8, /* detect ctl request 3 */
    (uint32 *) 0x600621F0, /* polarity request */
    (uint32 *) 0x600621F8, /* status */
    0x0,
    send_pronto_wakeup_interrupt,
    send_pronto_vmpm_interrupt,
    PRONTO_IRQ },

  /* TZ */
  { (uint32 *) 0x600631D8, /* enable request */
    (uint32 *) 0x600631E0, /* detect ctl request */
    (uint32 *) 0x600631E8, /* detect ctl request 3 */
    (uint32 *) 0x600631F0, /* polarity request */
    (uint32 *) 0x600631F8, /* status */
    0x6,
    NULL,
    NULL,
    PRONTO_IRQ },

  /* QDSP */
  { (uint32 *) 0x600641D8, /* enable request */
    (uint32 *) 0x600641E0, /* detect ctl request */
    (uint32 *) 0x600641E8, /* detect ctl request 3 */
    (uint32 *) 0x600641F0, /* polarity request */
    (uint32 *) 0x600641F8, /* status */
    0x2,
    send_lpass_wakeup_interrupt,
    send_lpass_vmpm_interrupt,
    LPASS_IRQ },
};

