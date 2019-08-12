#include <assert.h>
#include <stdbool.h>

#include "cortex-m3.h"
#include "HALhwio.h"
#include "msmhwiobase.h"
//#include "qdss_hwio.h"

#include "CoreVerify.h"
#include "npa.h"

#define QDSS_CLK_IRQ    35
#define QDSS_PWRUP_IRQ  36
#define QDSS_JTAGSW_IRQ 37

enum
{
  RPM_SERVER_INIT_DONE ,
  RPM_INIT_DONE,
};

static npa_client_handle debug_qdss_client     = 0;
static bool              debug_pwrup_req_state = false;
static bool              debug_jtagsw_state    = false;

static void qdss_grand_override_isr(void) __irq
{
//    unsigned vote = HWIO_IN(QDSS_CS_QDSSCSR_QDSSCLKVOTE) & 3;
    unsigned vote =  3;
    npa_issue_required_request(debug_qdss_client, vote);
}

static void qdss_pwrup_req_isr(void) __irq
{
    debug_pwrup_req_state = !debug_pwrup_req_state;
    interrupt_configure(QDSS_PWRUP_IRQ, debug_pwrup_req_state ? LEVEL_LOW : LEVEL_HIGH);
}

static void qdss_jtagsw_isr(void) __irq
{
    debug_jtagsw_state = !debug_jtagsw_state;
    interrupt_configure(QDSS_JTAGSW_IRQ, debug_jtagsw_state ? LEVEL_LOW : LEVEL_HIGH);
}

// Right now, just support for the main QDSS debugger signals.
// Eventually, support for ITM/ETM/DWT as well.
void debug_init(void)
{
    debug_qdss_client = npa_create_sync_client("/clk/qdss", "debugger", NPA_CLIENT_REQUIRED);
    CORE_VERIFY(debug_qdss_client);

    // The QDSS "grand override" is a way for the debugger to vote on the QDSS clock.
    interrupt_set_isr(QDSS_CLK_IRQ, qdss_grand_override_isr);
    interrupt_configure(QDSS_CLK_IRQ, RISING_EDGE);
    interrupt_enable(QDSS_CLK_IRQ);

    // These next two signals are sent by the debugger to request that the chip stay debuggable.
    interrupt_set_isr(QDSS_PWRUP_IRQ, qdss_pwrup_req_isr);
    interrupt_configure(QDSS_PWRUP_IRQ, RISING_EDGE);
    interrupt_enable(QDSS_PWRUP_IRQ);

    interrupt_set_isr(QDSS_JTAGSW_IRQ, qdss_jtagsw_isr);
    interrupt_configure(QDSS_JTAGSW_IRQ, RISING_EDGE);
    interrupt_enable(QDSS_JTAGSW_IRQ);
}

bool debugger_connected(void)
{
    //return debug_pwrup_req_state || debug_jtagsw_state;
    return false;
}

#define SHARED_IMEM_BASE 0x68600000
#define RESERVED_IMEM_DEBUG_COOKIE  (SHARED_IMEM_BASE + 0x00000B18)
#ifdef COOKIE_ADDR_IN_TCSR
    #include "tcsr_hwio.h"
    #define RPM_INIT_COOKIE             (HWIO_TCSR_SPARE_REG0_ADDR)
#else
    #define RPM_INIT_COOKIE             (SHARED_IMEM_BASE + 0x00000B1C)
#endif

#define DEBUG_COOKIE_VALUE          0x53444247


void debug_cookie_init(void)
{
   unsigned int cookie = 0;

   do
   {
      cookie =   (*((volatile unsigned int *) (RESERVED_IMEM_DEBUG_COOKIE)));
   }while(cookie == DEBUG_COOKIE_VALUE);

}

void rpm_init_done(void)
{

    unsigned *cookie = (unsigned *)(RPM_INIT_COOKIE);
    *cookie |= (1 << RPM_INIT_DONE);

}

void rpm_server_init_done(void)
{

    unsigned *cookie = (unsigned *)(RPM_INIT_COOKIE);
    *cookie = (1 << RPM_SERVER_INIT_DONE);

}
