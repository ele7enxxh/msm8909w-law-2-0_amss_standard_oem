#include <stdint.h>

static volatile uint32_t * const RPM_QTMR_F0_CNTP_CTL = (volatile uint32_t *)0x008302C;
static volatile uint32_t * const RPM_QTMR_F1_CNTP_CTL = (volatile uint32_t *)0x008402C;

void time_service_init(void)
{
    *RPM_QTMR_F0_CNTP_CTL = 0x3; // enable with interrupt masked
    *RPM_QTMR_F1_CNTP_CTL = 0x3; // enable with interrupt masked
}

